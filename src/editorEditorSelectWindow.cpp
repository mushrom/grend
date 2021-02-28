#include <grend/gameEditor.hpp>
#include <grend/utility.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <string.h>

using namespace grendx;

static char searchBuffer[0x1000] = "";
static bool showAddEntityWindow = false;

static void drawSelectableLabel(const char *txt) {
	if (ImGui::Selectable(txt)) {
		if (*searchBuffer) {
			// add a space if the search buffer already has contents
			strncat(searchBuffer, " ", sizeof(searchBuffer) - 1);
		}

		strncat(searchBuffer, txt, sizeof(searchBuffer) - 1);
	}
}

// TODO: might want to expose this in the menubar
static void addEntityWindow(gameMain *game, bool *show) {
	static char comboBuf[0x1000];
	static const size_t bufsize = sizeof(comboBuf) - 1;

	*comboBuf = '\0'; // reset every frame

	ImGui::Begin("Add entity", show);
	std::vector<std::string> names;

	size_t n = 0;
	for (auto& [name, _] : game->factories->factories) {
		names.push_back(name);

		if (n + name.length() + 2 < bufsize) {
			strncpy(comboBuf + n, name.c_str(), bufsize - n);
			n += name.length() + 1;
			// add second null terminator in case this is the last entry
			comboBuf[n + 1] = '\0';
		}
	}

	// make sure the combo string is null-terminated (man I don't miss C strings)
	// this is one big thing I'm not a fan of in imgui -
	// std::initializer_list would still be pretty cheap for the cases where you
	// want to specify a list of constants in code, std::vector& would be _much_
	// cheaper for cases like this where you're dynamically generating some list
	// of options...
	// why not use a more c++-ish style, it's basically a c library written in c++
	comboBuf[bufsize] = '\0';
	comboBuf[bufsize - 1] = '\0';

	static int baseIndex;
	static std::vector<int> componentIndexes;

	ImGui::Combo("Base entity", &baseIndex, comboBuf);
	ImGui::Separator();

	float footer = ImGui::GetStyle().ItemSpacing.y
		+ ImGui::GetFrameHeightWithSpacing();
	float sidebuttons = ImGui::GetStyle().ItemSpacing.x
		+ ImGui::GetFrameHeightWithSpacing();
	
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, sidebuttons);
	ImGui::BeginChild("buttons", ImVec2(sidebuttons, -footer));
	if (ImGui::Button("+")) {
		componentIndexes.push_back(0);
	}

	if (ImGui::Button("-") && !componentIndexes.empty()) {
		if (!componentIndexes.empty()) {
			componentIndexes.pop_back();
		}
	}
	ImGui::EndChild();
	ImGui::NextColumn();

	ImGui::BeginChild("components", ImVec2(0, -footer));
	for (unsigned i = 0; i < componentIndexes.size(); i++) {
		std::string compstr = "attachment " + std::to_string(i);
		std::string popupstr = "popup " + std::to_string(i);

		ImGui::Combo(compstr.c_str(), componentIndexes.data() + i, comboBuf);

		if (ImGui::BeginPopupContextItem(popupstr.c_str())) {
			ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.f), "Remove?");
			ImGui::Separator();

			if (ImGui::Selectable("Yes")) {
				componentIndexes.erase(componentIndexes.begin() + i);
			}

			if (ImGui::Selectable("No")) { /* Nop */ }
			ImGui::EndPopup();
		}
	}
	ImGui::EndChild();
	ImGui::Columns(1);
	ImGui::Separator();

	if (ImGui::Button("OK")) {
		// TODO:
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		componentIndexes.clear();
		baseIndex = 0;
		*show = false;
	}

	ImGui::End();
}

void gameEditor::entitySelectWindow(gameMain *game) {
	ImGui::Begin("Entities", &showEntitySelectWindow);
	if (ImGui::Button("Clear")) {
		*searchBuffer = '\0';
	}
	ImGui::SameLine();
	ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer));
	auto tags = split_string(searchBuffer);

	ImGui::SameLine();
	if (ImGui::Button("New entity")) {
		showAddEntityWindow = true;
	}
	ImGui::Separator();

	ImGui::Columns(2);
	ImGui::BeginChild("components");
	ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.f), "Active components");
	ImGui::Separator();

	ImGui::BeginChild("componentList");
	for (const auto& [name, _] : game->entities->components) {
		drawSelectableLabel(name.c_str());
	}

	ImGui::EndChild();
	ImGui::EndChild();
	ImGui::NextColumn();

	ImGui::BeginChild("matchingEnts", ImVec2(0, 0), false, 0);
	ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.f), "Matching entities");
	ImGui::Separator();

	ImGui::BeginChild("entityList", ImVec2(0, 0), false, 0);
	for (auto& ent : game->entities->entities) {
		if (*searchBuffer && !game->entities->hasComponents(ent, tags)) {
			// entity doesn't have the searched tags, filtered out
			continue;
		}

		std::string entstr = "entity #" + std::to_string((uintptr_t)ent);
		std::string contextstr = entstr + ":context";

		if (game->entities->condemned.count(ent)) {
			entstr = "[deleted] " + entstr;
		}

		if (ImGui::Selectable(entstr.c_str(), ent == selectedEntity)) {
			selectedEntity = ent;
			selectedNode = ent->node;
		}

		if (ImGui::BeginPopupContextItem(contextstr.c_str())) {
			ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.f), "Action");
			ImGui::Separator();

			if (ImGui::Selectable("Delete")) {
				game->entities->remove(selectedEntity);
				selectedEntity = nullptr;
			}

			if (ImGui::Selectable("Duplicate")) { /* TODO */ }
			ImGui::EndPopup();
		}

		auto& components = game->entities->getEntityComponents(ent);
		std::set<std::string> seen;
		ImGui::Separator();
		ImGui::Indent(16.f);
		ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.f), "Attached components:");
		ImGui::Columns(2);

		std::string sectionName = entstr + ":components";
		ImGui::TreePush(sectionName.c_str());

		for (auto& [name, comp] : components) {
			if (!seen.count(name)) {
				drawSelectableLabel(name.c_str());
				ImGui::NextColumn();
				seen.insert(name);
			}
		}

		ImGui::TreePop();
		ImGui::Columns(1);
		ImGui::Unindent(16.f);
		ImGui::Separator();

		// TODO: add/remove entity
	}

	ImGui::EndChild();
	ImGui::EndChild();
	ImGui::Columns(1);
	ImGui::End();

	if (showAddEntityWindow) {
		addEntityWindow(game, &showAddEntityWindow);
	}
}
