#include <grend/game_editor.hpp>
#include <grend/file_dialog.hpp>

// TODO: debugging, remove
#include <iostream>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_sdl.h>
#include <imgui/examples/imgui_impl_opengl3.h>

using namespace grendx;

static file_dialog open_dialog("Open File");
static file_dialog save_as_dialog("Save As...");

static file_dialog import_model_dialog("Import Model");
static file_dialog import_scene_dialog("Import Scene");
static file_dialog import_map_dialog("Import Map");

static void handle_prompts(game_editor *editor, renderer *rend) {
	if (open_dialog.prompt_filename()) {
		std::cout << "Opening a file here! at " << open_dialog.selection <<  std::endl;
		open_dialog.clear();

		editor->clear(rend);
		editor->load_map(rend, open_dialog.selection);
	}

	if (save_as_dialog.prompt_filename()) {
		std::cout << "Saving as a file! at " << save_as_dialog.selection << std::endl;

		editor->save_map(rend, save_as_dialog.selection);
		save_as_dialog.clear();
	}

	if (import_model_dialog.prompt_filename()) {
		std::cout << "Importing a thing! at "
		          << import_model_dialog.selection << std::endl;
		import_model_dialog.clear();

		// TODO: XXX: no need for const if it's just being casted away anyway...
		auto& glman = (gl_manager&)rend->get_glman();
		editor->load_model(rend, import_model_dialog.selection);
		glman.bind_cooked_meshes();
		editor->update_models(rend);
	}

	if (import_scene_dialog.prompt_filename()) {
		std::cout << "Importing a scene! at "
		          << import_scene_dialog.selection << std::endl;
		import_scene_dialog.clear();

		auto& glman = (gl_manager&)rend->get_glman();
		editor->load_scene(rend, import_scene_dialog.selection);
		glman.bind_cooked_meshes();
		editor->update_models(rend);
	}

	if (import_map_dialog.prompt_filename()) {
		std::cout << "Importing a map! at "
		          << import_map_dialog.selection << std::endl;
		import_scene_dialog.clear();
	}
}

void game_editor::menubar(renderer *rend) {
	static bool demo_window = false;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", "CTRL+O")) { open_dialog.show(); }
			if (ImGui::MenuItem("Save", "CTRL+S")) {}
			if (ImGui::MenuItem("Save As", "Shift+CTRL+S")) { save_as_dialog.show(); }

			if (ImGui::MenuItem("Close", "CTRL+O")) {}

			ImGui::Separator();
			//if (ImGui::MenuItem("Import")) { import_dialog.show(); }
			if (ImGui::BeginMenu("Import")) {
				if (ImGui::MenuItem("Model(s)")) { import_model_dialog.show(); }
				if (ImGui::MenuItem("Scene")) { import_scene_dialog.show(); }
				if (ImGui::MenuItem("Map")) { import_map_dialog.show(); }
				ImGui::EndMenu();
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Reload shaders", "CTRL+R")){ reload_shaders(rend); }

			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "CTRL+Q")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y")) {}

			ImGui::Separator();
			if (ImGui::MenuItem("Reset+Regenerate physics", "CTRL+P")) {}

			ImGui::Separator();
			if (ImGui::MenuItem("asdf", "CTRL+V")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options")) {
			ImGui::SliderFloat("Snap increment", &fidelity,
			                  0.5f, 50.f, "%.1f");
			ImGui::SliderFloat("Movement speed", &movement_speed,
			                  1.f, 100.f, "%.1f");
			ImGui::SliderFloat("Exposure (tonemapping)", &exposure, 0.1, 10.f);
			ImGui::SliderFloat("Light threshold", &light_threshold,
			                   0.001, 1.f);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools")) {
			if (ImGui::MenuItem("Map editor", "o"))
				show_map_window = true;
			if (ImGui::MenuItem("Lights editor", "l"))
				show_lights_window = true;
			if (ImGui::MenuItem("Reflection probes", "r"))
				show_refprobe_window = true;
			if (ImGui::MenuItem("Object selection"))
				show_object_select_window = true;

			if (ImGui::MenuItem("Material editor", "CTRL+M")) {}

			ImGui::Separator();
			if (ImGui::MenuItem("Add object", "r")) {}
			if (ImGui::MenuItem("Scale object", "s")) {}

			ImGui::Separator();
			if (ImGui::MenuItem("Bake lighting", "CTRL+B")) {}
			if (ImGui::MenuItem("Generate environment light probes", "CTRL+L")) {}
			if (ImGui::MenuItem("Generate IBL cubemaps", "Shift-CTRL+L")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug")) {
			ImGui::MenuItem("Dear ImGui demo window", NULL, &demo_window);
			ImGui::EndMenu();
		}

		ImGui::Combo("[mode]", &mode,
			"Exit editor\0" "View\0" "Add object\0"
			"Add point light\0" "Add spot light\0" "Add directional light\0"
			"Add reflection probe\0"
			"Select\0"
			"\0");

		ImGui::EndMainMenuBar();
	}

	if (demo_window) {
		ImGui::ShowDemoWindow(&demo_window);
	}

	handle_prompts(this, rend);
}
