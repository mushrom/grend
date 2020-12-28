#include <grend/gameEditor.hpp>

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_sdl.h>
#include <imgui/examples/imgui_impl_opengl3.h>

using namespace grendx;

void gameEditor::objectSelectWindow(gameMain *game) {
	ImGui::Begin("Loaded Objects", &showObjectSelectWindow);

	// TODO: another way to select from loaded models
	for (auto it = models.begin(); it != models.end(); it++) {
		auto& [name, obj] = *it;

		if (ImGui::Selectable(name.c_str(), obj == selectedNode)) {
			selectedNode = obj;
		}

		ImGui::SameLine(300);
		std::string mstr = "(" + std::to_string(obj->nodes.size()) + " meshes)";
		ImGui::Text(mstr.c_str());
	}

	ImGui::End();
}