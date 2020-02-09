#include <grend/engine.hpp>
#include <grend/model.hpp>
#include <vector>
#include <map>
#include <iostream>

using namespace grendx;

static std::map<std::string, material> default_materials = {
	{"(null)", {
				   .diffuse = {0.75, 0.75, 0.75, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {0.5, 0.5, 0.5, 1},
				   .shininess = 15,

				   .diffuse_map     = "assets/tex/white.png",
				   .specular_map    = "assets/tex/white.png",
				   .normal_map      = "assets/tex/lightblue-normal.png",
				   .ambient_occ_map = "assets/tex/white.png",
			   }},

	{"Black",  {
				   .diffuse = {1, 0.8, 0.2, 1},
				   .ambient = {1, 0.8, 0.2, 1},
				   .specular = {1, 1, 1, 1},
				   .shininess = 50
			   }},

	{"Grey",   {
				   .diffuse = {0.1, 0.1, 0.1, 0.5},
				   .ambient = {0.1, 0.1, 0.1, 0.2},
				   .specular = {0.0, 0.0, 0.0, 0.05},
				   .shininess = 15
			   }},

	{"Yellow", {
				   .diffuse = {0.01, 0.01, 0.01, 1},
				   .ambient = {0, 0, 0, 1},
				   .specular = {0.2, 0.2, 0.2, 0.2},
				   .shininess = 20,
			   }},

	{"Gravel",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 1},
				   .shininess = 5,

				   .diffuse_map  = "assets/tex/dims/Textures/Gravel.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/dims/Textures/Textures_N/Gravel_N.jpg",

				   // materials from freepbr.com are way higher quality, but I'm not sure how
				   // github would feel about hosting a few hundred megabytes to gigabytes of
				   // textures

				   /*
				   .diffuse_map  = "assets/tex/octostone-Unreal-Engine/octostoneAlbedo.png",
				   .specular_map = "assets/tex/octostone-Unreal-Engine/octostoneMetallic.png",
				   .normal_map   = "assets/tex/octostone-Unreal-Engine/octostoneNormalc.png",
				   .ambient_occ_map = "assets/tex/octostone-Unreal-Engine/octostoneAmbient_Occlusionc.png",
				   */

				   /*
				   .diffuse_map  = "assets/tex/octostone-Unreal-Engine/256px-octostoneAlbedo.png",
				   .specular_map = "assets/tex/octostone-Unreal-Engine/256px-octostoneMetallic.png",
				   .normal_map   = "assets/tex/octostone-Unreal-Engine/256px-octostoneNormalc.png",
				   .ambient_occ_map = "assets/tex/octostone-Unreal-Engine/256px-octostoneAmbient_Occlusionc.png",
				   */

				   /*
				   .diffuse_map  = "assets/tex/octostone-Unreal-Engine/128px-octostoneAlbedo.png",
				   .specular_map = "assets/tex/octostone-Unreal-Engine/128px-octostoneMetallic.png",
				   .normal_map   = "assets/tex/octostone-Unreal-Engine/128px-octostoneNormalc.png",
				   .ambient_occ_map = "assets/tex/octostone-Unreal-Engine/128px-octostoneAmbient_Occlusionc.png",
				   */
			   }},

	{"Steel",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 1},
				   .shininess = 35,

				   .diffuse_map  = "assets/tex/rubberduck-tex/199.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/rubberduck-tex/199_norm.JPG",
				   .ambient_occ_map = "assets/tex/white.png",

				   /*
				   .diffuse_map  = "assets/tex/iron-rusted4-Unreal-Engine/iron-rusted4-basecolor.png",
				   .specular_map = "assets/tex/iron-rusted4-Unreal-Engine/iron-rusted4-metalness.png",
				   .normal_map   = "assets/tex/iron-rusted4-Unreal-Engine/iron-rusted4-normal.png",
				   */
			   }},

	{"Brick",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 1},
				   .shininess = 3,

				   .diffuse_map  = "assets/tex/rubberduck-tex/179.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/rubberduck-tex/179_norm.JPG",
				   .ambient_occ_map = "assets/tex/white.png",
				   /*
				   .diffuse_map  = "assets/tex/dims/Textures/Chimeny.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/dims/Textures/Textures_N/Chimeny_N.jpg",
				   */
			   }},


	{"Rock",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 0.5},
				   .shininess = 5,

				   .diffuse_map  = "assets/tex/rubberduck-tex/165.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/rubberduck-tex/165_norm.JPG",
				   .ambient_occ_map = "assets/tex/white.png",
			   }},

	{"Wood",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 0.3},
				   .shininess = 5,

				   .diffuse_map  = "assets/tex/dims/Textures/Boards.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/dims/Textures/Textures_N/Boards_N.jpg",
				   .ambient_occ_map = "assets/tex/white.png",
			   }},

	{"Clover",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 0.5},
				   .shininess = 1,

				   .diffuse_map  = "assets/tex/dims/Textures/GroundCover.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/dims/Textures/Textures_N/GroundCover_N.jpg",
				   .ambient_occ_map = "assets/tex/white.png",
			   }},
};

engine::engine() {
	for (auto& thing : default_materials) {
		if (!thing.second.diffuse_map.empty()) {
			diffuse_handles[thing.first] = glman.load_texture(thing.second.diffuse_map);
		}

		if (!thing.second.specular_map.empty()) {
			specular_handles[thing.first] = glman.load_texture(thing.second.specular_map);
		}

		if (!thing.second.normal_map.empty()) {
			normmap_handles[thing.first] = glman.load_texture(thing.second.normal_map);
		}

		if (!thing.second.ambient_occ_map.empty()) {
			aomap_handles[thing.first] = glman.load_texture(thing.second.ambient_occ_map);
		}
	}
}

void engine::set_material(gl_manager::compiled_model& obj, std::string mat_name) {
	if (obj.materials.find(mat_name) == obj.materials.end()) {
		// TODO: maybe show a warning
		set_default_material(mat_name);
		//std::cerr << " ! couldn't find material " << mat_name << std::endl;
		return;
	}

	material& mat = obj.materials[mat_name];

	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.diffuse"),
			mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, mat.diffuse.w);
	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.ambient"),
			mat.ambient.x, mat.ambient.y, mat.ambient.z, mat.ambient.w);
	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.specular"),
			mat.specular.x, mat.specular.y, mat.specular.z, mat.specular.w);
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.shininess"),
			mat.shininess);

	glActiveTexture(GL_TEXTURE0);
	if (!mat.diffuse_map.empty()) {
		glBindTexture(GL_TEXTURE_2D, obj.mat_textures[mat_name].first);
		glUniform1i(u_diffuse_map, 0);

	} else {
		glBindTexture(GL_TEXTURE_2D, diffuse_handles[fallback_material].first);
		glUniform1i(u_diffuse_map, 0);
	}

	glActiveTexture(GL_TEXTURE1);
	if (!mat.specular_map.empty()) {
		// TODO: specular maps
		glBindTexture(GL_TEXTURE_2D, obj.mat_specular[mat_name].first);
		glUniform1i(u_specular_map, 1);

	} else {
		glBindTexture(GL_TEXTURE_2D, specular_handles[fallback_material].first);
		glUniform1i(u_specular_map, 1);
	}

	glActiveTexture(GL_TEXTURE2);
	if (!mat.normal_map.empty()) {
		glBindTexture(GL_TEXTURE_2D, obj.mat_normal[mat_name].first);
		glUniform1i(u_normal_map, 2);

	} else {
		glBindTexture(GL_TEXTURE_2D, normmap_handles[fallback_material].first);
		glUniform1i(u_normal_map, 2);
	}

	glActiveTexture(GL_TEXTURE3);
	if (!mat.ambient_occ_map.empty()) {
		glBindTexture(GL_TEXTURE_2D, obj.mat_ao[mat_name].first);
		glUniform1i(u_ao_map, 3);

	} else {
		glBindTexture(GL_TEXTURE_2D, aomap_handles[fallback_material].first);
		glUniform1i(u_ao_map, 3);
	}
}

void engine::set_default_material(std::string mat_name) {
	if (default_materials.find(mat_name) == default_materials.end()) {
		// TODO: really show an error here
		mat_name = fallback_material;
		puts("asdf");
	}

	material& mat = default_materials[mat_name];

	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.diffuse"),
			mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, mat.diffuse.w);
	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.ambient"),
			mat.ambient.x, mat.ambient.y, mat.ambient.z, mat.ambient.w);
	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.specular"),
			mat.specular.x, mat.specular.y, mat.specular.z, mat.specular.w);
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.shininess"),
			mat.shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_handles[mat.diffuse_map.empty()?
	                                                 fallback_material
	                                                 : mat_name].first);
	glUniform1i(u_diffuse_map, 0);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular_handles[mat.specular_map.empty()?
	                                                 fallback_material
	                                                 : mat_name].first);
	glUniform1i(u_specular_map, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normmap_handles[mat.normal_map.empty()?
	                                                 fallback_material
	                                                 : mat_name].first);
	glUniform1i(u_normal_map, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, aomap_handles[mat.ambient_occ_map.empty()?
	                                                 fallback_material
	                                                 : mat_name].first);
	glUniform1i(u_ao_map, 3);
}

void engine::set_mvp(glm::mat4 mod, glm::mat4 view, glm::mat4 projection) {
	glm::mat4 v_inv = glm::inverse(view);

	glUniformMatrix4fv(glGetUniformLocation(shader.first, "v"),
			1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.first, "p"),
			1, GL_FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(glGetUniformLocation(shader.first, "v_inv"),
			1, GL_FALSE, glm::value_ptr(v_inv));
}

static glm::mat4 model_to_world(glm::mat4 model) {
	glm::quat rotation = glm::quat_cast(model);
	//rotation = -glm::conjugate(rotation);

	return glm::mat4_cast(rotation);
}

void engine::set_m(glm::mat4 mod) {
	glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(model_to_world(mod)));

	glUniformMatrix4fv(glGetUniformLocation(shader.first, "m"),
			1, GL_FALSE, glm::value_ptr(mod));
	glUniformMatrix3fv(glGetUniformLocation(shader.first, "m_3x3_inv_transp"),
			1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
}

void engine::draw_mesh(std::string name, glm::mat4 transform) {
	gl_manager::compiled_mesh& foo = glman.cooked_meshes[name];
	set_m(transform);

	glman.bind_vao(foo.vao);
	glDrawElements(GL_TRIANGLES, foo.elements_size, GL_UNSIGNED_SHORT, foo.elements_offset);
}

// TODO: overload of this that takes a material
void engine::draw_mesh_lines(std::string name, glm::mat4 transform) {
	gl_manager::compiled_mesh& foo = glman.cooked_meshes[name];

	set_m(transform);
	glman.bind_vao(foo.vao);

	glDrawElements(GL_LINES, foo.elements_size, GL_UNSIGNED_SHORT, foo.elements_offset);
}

void engine::draw_model(std::string name, glm::mat4 transform) {
	gl_manager::compiled_model& obj = glman.cooked_models[name];

	for (std::string& name : obj.meshes) {
		set_material(obj, glman.cooked_meshes[name].material);
		draw_mesh(name, transform);
	}
}

void engine::draw_model_lines(std::string name, glm::mat4 transform) {
	gl_manager::compiled_model& obj = glman.cooked_models[name];
	// TODO: need a set_material() function to handle stuff
	//       and we need to explicitly set a material

	set_default_material("(null)");
	for (std::string& name : obj.meshes) {
		draw_mesh_lines(name, transform);
	}
}

bool engine::is_valid_light(int id) {
	return id < MAX_LIGHTS && id >= 0 && lights[id].is_active;
}

int engine::add_light(struct light lit) {
	int ret = -1;

	// TODO: allocated light bitmap
	for (unsigned i = 0; i < MAX_LIGHTS; i++) {
		if (!lights[i].is_active) {
			ret = i;
			break;
		}
	}

	if (ret >= 0) {
		lights[ret] = lit;
		lights[ret].changed = true;
		lights[ret].is_active = true;
	}

	return ret;
}

int engine::set_light(int id, struct light lit) {
	if (is_valid_light(id)) {
		lights[id] = lit;
		lights[id].changed = true;
		return id;
	}

	return -1;
}

int engine::get_light(int id, struct light *lit) {
	if (is_valid_light(id)) {
		*lit = lights[id];
		return id;
	}

	return -1;
}


void engine::remove_light(int id) {
	if (is_valid_light(id)) {
		lights[id].is_active = false;
		lights[id].changed = true;
	}
}

void engine::init_lights(void) {
	for (unsigned i = 0; i < MAX_LIGHTS; i++) {
		// things should have been set to zero during construction, I'm pretty sure
		/*
		lights[i].position = glm::vec4(i, 5, 0, 1);
		lights[i].diffuse  = glm::vec4(0.5 + i/(float)MAX_LIGHTS/2.f, 1.0, 0.5 + i/(float)MAX_LIGHTS/2.f, 0.0);
		lights[i].const_attenuation = 1.0f;
		lights[i].specular = 1.0;
		lights[i].quadratic_attenuation = 0.02f;
		lights[i].is_active = true;
		*/
		lights[i].changed   = true;
	}

	update_lights();
}

void engine::sync_light(unsigned id) {
	if (id >= MAX_LIGHTS) {
		std::cerr << "/!\\ light " << id << " out of bounds" << std::endl;
		return;
	}

	// TODO: cache uniform locations (good idea: add a caching layer to glman)
	// TODO: also updating all these uniforms can't be very efficient for a lot of
	//       moving point lights... maybe look into how uniform buffer objects work
	std::string locstr = "lights[" + std::to_string(id) + "]";
	std::map<std::string, GLint> light_handles;

	for (std::string str : {
			"position", "diffuse", "const_attenuation",
			"linear_attenuation", "quadratic_attenuation",
			"specular", "is_active"
	}) {
		light_handles[str] = glGetUniformLocation(shader.first, (locstr + "." + str).c_str());
		DO_ERROR_CHECK();
		if (light_handles[str] == -1) {
			std::cerr << "/!\\ couldn't find " + locstr + "." + str << std::endl;
		}
	}

	glUniform4fv(light_handles["position"], 1, glm::value_ptr(lights[id].position));
	DO_ERROR_CHECK();
	glUniform4fv(light_handles["diffuse"], 1, glm::value_ptr(lights[id].diffuse));
	DO_ERROR_CHECK();

	glUniform1f(light_handles["const_attenuation"], lights[id].const_attenuation);
	DO_ERROR_CHECK();
	glUniform1f(light_handles["linear_attenuation"], lights[id].linear_attenuation);
	DO_ERROR_CHECK();
	glUniform1f(light_handles["quadratic_attenuation"], lights[id].quadratic_attenuation);
	DO_ERROR_CHECK();
	glUniform1f(light_handles["specular"], lights[id].specular);
	DO_ERROR_CHECK();
	glUniform1i(light_handles["is_active"], lights[id].is_active);
	DO_ERROR_CHECK();
}

void engine::update_lights(void) {
	// TODO: sync max light constants
	for (unsigned i = 0; i < MAX_LIGHTS; i++) {
		auto& lit = lights[i];

		if (lit.changed) {
			sync_light(i);
			lit.changed = false;
		}
	}
}
