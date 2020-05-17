#include <grend/engine.hpp>
#include <grend/model.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <grend/utility.hpp>

using namespace grendx;

static std::map<std::string, material> default_materials = {
	{"(null)", {
				   .diffuse = {0.75, 0.75, 0.75, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {0.5, 0.5, 0.5, 1},
				   .roughness = 0.5,
				   .metalness = 0.5,
				   .opacity = 1,
				   .refract_idx = 1.5,

				   .diffuse_map     = material_texture("assets/tex/white.png"),
				   .metal_roughness_map    = material_texture("assets/tex/white.png"),
				   .normal_map      = material_texture("assets/tex/lightblue-normal.png"),
				   .ambient_occ_map = material_texture("assets/tex/white.png"),
			   }},

	{"Black",  {
				   .diffuse = {1, 0.8, 0.2, 1},
				   .ambient = {1, 0.8, 0.2, 1},
				   .specular = {1, 1, 1, 1},
				   .roughness = 0.5,
				   .metalness = 0.5,
				   .opacity = 1,
				   .refract_idx = 1.5,
			   }},

	{"Grey",   {
				   .diffuse = {0.1, 0.1, 0.1, 0.5},
				   .ambient = {0.1, 0.1, 0.1, 0.2},
				   .specular = {0.0, 0.0, 0.0, 0.05},
				   .roughness = 0.75,
				   .metalness = 0.5,
				   .opacity = 1,
				   .refract_idx = 1.5,
			   }},

	{"Yellow", {
				   .diffuse = {0.01, 0.01, 0.01, 1},
				   .ambient = {0, 0, 0, 1},
				   .specular = {0.2, 0.2, 0.2, 0.2},
				   .roughness = 0.3,
				   .metalness = 0.5,
				   .opacity = 1,
				   .refract_idx = 1.5,
			   }},

	{"Steel",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 1},
				   .roughness = 0.2,
				   .metalness = 0.5,
				   .opacity = 1,
				   .refract_idx = 1.5,

				   /*
				   .diffuse_map  = "assets/tex/rubberduck-tex/199.JPG",
				   .specular_map = "assets/tex/white.png",
				   .normal_map   = "assets/tex/rubberduck-tex/199_norm.JPG",
				   .ambient_occ_map = "assets/tex/white.png",
				   */

				   .diffuse_map  = material_texture("assets/tex/iron-rusted4-Unreal-Engine/iron-rusted4-basecolor.png"),
				   .metal_roughness_map = material_texture("assets/tex/iron-rusted4-Unreal-Engine/iron-rusted4-metalness.png"),
				   .normal_map   = material_texture("assets/tex/iron-rusted4-Unreal-Engine/iron-rusted4-normal.png"),
			   }},

	{"Wood",  {
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 0.3},
				   .roughness = 0.7,
				   .metalness = 0.5,
				   .opacity = 1,
				   .refract_idx = 1.5,

				   .diffuse_map = material_texture("assets/tex/white.png"),
				   //.diffuse_map  = "assets/tex/rubberduck-tex/199.JPG",
				   //.diffuse_map  = material_texture("assets/tex/dims/Textures/Boards.JPG"),
				   .metal_roughness_map = material_texture("assets/tex/white.png"),
				   //.normal_map   = material_texture("assets/tex/dims/Textures/Textures_N/Boards_N.jpg"),
				   .normal_map = material_texture("assets/tex/white.png"),
				   //.normal_map   = "assets/tex/rubberduck-tex/199_norm.JPG",
				   .ambient_occ_map = material_texture("assets/tex/white.png"),
			   }},

	{"Glass",  {
				   //.diffuse = {0.2, 0.5, 0.2, 0},
				   .diffuse = {0.5, 0.5, 0.5, 0},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 0.5},
				   .roughness = 0.05,
				   .metalness = 0.5,
				   .opacity = 0.1,
				   .refract_idx = 1.5,
			   }},

	{"Earth",  {
				   //.diffuse = {0.2, 0.5, 0.2, 0},
				   .diffuse = {1, 1, 1, 1},
				   .ambient = {1, 1, 1, 1},
				   .specular = {1, 1, 1, 0.5},
				   .roughness = 0.5,
				   .metalness = 0.5,
				   .opacity = 1.0,
				   .refract_idx = 1.5,

				   //.diffuse_map  = material_texture("assets/tex/Earthmap720x360_grid.jpg"),
				   .diffuse_map = material_texture("assets/tex/white.png"),
			   }},
};

engine::engine() {
	for (auto& thing : default_materials) {
		std::cerr << __func__ << ": loading materials for "
			<< thing.first << std::endl;

		if (thing.second.diffuse_map.loaded()) {
			diffuse_handles[thing.first] =
				glman.buffer_texture(thing.second.diffuse_map, true /* srgb */);
		}

		if (thing.second.metal_roughness_map.loaded()) {
			specular_handles[thing.first] =
				glman.buffer_texture(thing.second.metal_roughness_map);
		}

		if (thing.second.normal_map.loaded()) {
			normmap_handles[thing.first] =
				glman.buffer_texture(thing.second.normal_map);
		}

		if (thing.second.ambient_occ_map.loaded()) {
			aomap_handles[thing.first] =
				glman.buffer_texture(thing.second.ambient_occ_map);
		}
	}

	std::cerr << __func__ << ": Reached end of constructor" << std::endl;
}

void engine::set_material(gl_manager::compiled_model& obj, std::string mat_name) {
	if (obj.materials.find(mat_name) == obj.materials.end()) {
		// TODO: maybe show a warning
		set_default_material(mat_name);
		//std::cerr << " ! couldn't find material " << mat_name << std::endl;
		return;
	}

	material& mat = obj.materials[mat_name];

	// TODO: cache uniform locations
	// TODO: only update changed uniforms
	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.diffuse"),
			mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, mat.diffuse.w);
	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.ambient"),
			mat.ambient.x, mat.ambient.y, mat.ambient.z, mat.ambient.w);
	glUniform4f(glGetUniformLocation(shader.first, "anmaterial.specular"),
			mat.specular.x, mat.specular.y, mat.specular.z, mat.specular.w);
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.roughness"),
			mat.roughness);
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.metalness"),
			mat.metalness);
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.opacity"),
			mat.opacity);

	glActiveTexture(GL_TEXTURE0);
	if (obj.mat_textures.find(mat_name) != obj.mat_textures.end()) {
		glBindTexture(GL_TEXTURE_2D, obj.mat_textures[mat_name].first);
		glUniform1i(u_diffuse_map, 0);

	} else {
		glBindTexture(GL_TEXTURE_2D, diffuse_handles[fallback_material].first);
		glUniform1i(u_diffuse_map, 0);
	}

	glActiveTexture(GL_TEXTURE1);
	if (obj.mat_specular.find(mat_name) != obj.mat_specular.end()) {
		// TODO: specular maps
		glBindTexture(GL_TEXTURE_2D, obj.mat_specular[mat_name].first);
		glUniform1i(u_specular_map, 1);

	} else {
		glBindTexture(GL_TEXTURE_2D, specular_handles[fallback_material].first);
		glUniform1i(u_specular_map, 1);
	}

	glActiveTexture(GL_TEXTURE2);
	if (obj.mat_normal.find(mat_name) != obj.mat_normal.end()) {
		glBindTexture(GL_TEXTURE_2D, obj.mat_normal[mat_name].first);
		glUniform1i(u_normal_map, 2);

	} else {
		glBindTexture(GL_TEXTURE_2D, normmap_handles[fallback_material].first);
		glUniform1i(u_normal_map, 2);
	}

	glActiveTexture(GL_TEXTURE3);
	if (obj.mat_ao.find(mat_name) != obj.mat_ao.end()) {
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
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.roughness"),
			mat.roughness);
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.metalness"),
			mat.metalness);
	glUniform1f(glGetUniformLocation(shader.first, "anmaterial.opacity"),
			mat.opacity);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse_handles[mat.diffuse_map.loaded()?
													 mat_name
	                                                 : fallback_material].first);
	glUniform1i(u_diffuse_map, 0);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular_handles[mat.metal_roughness_map.loaded()?
													 mat_name
	                                                 : fallback_material].first);
	glUniform1i(u_specular_map, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normmap_handles[mat.normal_map.loaded()?
													 mat_name
	                                                 : fallback_material].first);
	glUniform1i(u_normal_map, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, aomap_handles[mat.ambient_occ_map.loaded()?
													 mat_name
	                                                 : fallback_material].first);
	glUniform1i(u_ao_map, 3);
}

void engine::set_mvp(glm::mat4 mod, glm::mat4 view, glm::mat4 projection) {
	glm::mat4 v_inv = glm::inverse(view);

	set_m(mod);

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
	DO_ERROR_CHECK();
	gl_manager::compiled_mesh& foo = glman.cooked_meshes[name];
	set_m(transform);
	DO_ERROR_CHECK();

	glman.bind_vao(foo.vao);
	glDrawElements(GL_TRIANGLES, foo.elements_size, GL_UNSIGNED_SHORT, foo.elements_offset);
}

// TODO: overload of this that takes a material
void engine::draw_mesh_lines(std::string name, glm::mat4 transform) {
	gl_manager::compiled_mesh& foo = glman.cooked_meshes[name];

	set_m(transform);
	glman.bind_vao(foo.vao);

#ifdef NO_GLPOLYMODE
	glDrawElements(GL_LINE_LOOP, foo.elements_size, GL_UNSIGNED_SHORT, foo.elements_offset);

#else
	// TODO: keep track of face culling state in this class
#ifdef ENABLE_FACE_CULLING
	glDisable(GL_CULL_FACE);
#endif

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, foo.elements_size, GL_UNSIGNED_SHORT, foo.elements_offset);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#ifdef ENABLE_FACE_CULLING
	glEnable(GL_CULL_FACE);
#endif
#endif
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

void engine::draw_screenquad(void) {
	// NOTE: assumes that the screenquad vbo has been linked in the
	//       postprocessing shader, and the proper vao set
	glDrawArrays(GL_TRIANGLES, 0, 6);
	DO_ERROR_CHECK();
}

void engine::dqueue_draw_model(std::string name, glm::mat4 transform) {
	draw_queue.push_back({name, transform});
}

void engine::dqueue_sort_draws(glm::vec3 camera) {
	typedef std::pair<std::string, glm::mat4> queue_ent;

	std::sort(draw_queue.begin(), draw_queue.end(),
		[&] (queue_ent a, queue_ent b) {
			glm::vec4 ta = a.second * glm::vec4(1);
			glm::vec4 tb = b.second * glm::vec4(1);
			glm::vec3 va = glm::vec3(ta) / ta.w;
			glm::vec3 vb = glm::vec3(tb) / tb.w;

			return glm::distance(camera, va) < glm::distance(camera, vb);
		});
}

void engine::dqueue_cull_models(glm::vec3 camera) {
	// TODO: filter objects that aren't visible, behind the camera
	//       or occluded by walls etc
}

void engine::dqueue_flush_draws(void) {
	for (auto& ent : draw_queue) {
		draw_model(ent.first, ent.second);
	}

	draw_queue.clear();
}

bool engine::is_valid_light(uint32_t id) {
	//return id < MAX_LIGHTS && id < (int)active_lights && id >= 0;
	return id && id <= light_ids && (
		point_lights.find(id) != point_lights.end()
		|| spot_lights.find(id) != spot_lights.end()
		|| directional_lights.find(id) != directional_lights.end()
	);
}

uint32_t engine::add_light(struct point_light lit) {
	uint32_t ret = alloc_light();
	point_lights[ret] = lit;
	return ret;
}

uint32_t engine::add_light(struct spot_light lit) {
	uint32_t ret = alloc_light();
	spot_lights[ret] = lit;
	return ret;
}

uint32_t engine::add_light(struct directional_light lit) {
	uint32_t ret = alloc_light();
	directional_lights[ret] = lit;
	return ret;
}

void engine::free_light(uint32_t id) {
	point_lights.erase(id);
	spot_lights.erase(id);
	directional_lights.erase(id);
}

/*
int engine::add_light(struct light lit) {
	int ret = -1;

	if (active_lights < MAX_LIGHTS) {
		ret = active_lights++;
		lights[ret] = lit;
		lights[ret].changed = true;
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
		for (unsigned i = id + 1; i < active_lights; i++) {
			lights[i-1] = lights[i];
			lights[i-1].changed = true;
		}

		active_lights--;
	}
}
*/

void engine::set_shader(gl_manager::rhandle& shd) {
	if (shader != shd) {
		shader = shd;
		glUseProgram(shader.first);
	}
}

// TODO: only update changed uniforms
void engine::sync_point_lights(const std::vector<uint32_t>& lights) {
	size_t active = min(MAX_LIGHTS, lights.size());
	glUniform1i(glGetUniformLocation(shader.first, "active_point_lights"), active);

	for (size_t i = 0; i < active; i++) {
		// TODO: check light index
		const auto& light = point_lights[lights[i]];

		// TODO: cache uniform locations (good idea: add a caching layer to glman)
		// TODO: also updating all these uniforms can't be very efficient for a lot of
		//       moving point lights... maybe look into how uniform buffer objects work
		std::string locstr = "point_lights[" + std::to_string(i) + "]";
		std::map<std::string, GLint> light_handles;

		for (std::string str : { "position", "diffuse", "radius", "intensity" }) {
			light_handles[str] =
				glGetUniformLocation(shader.first, (locstr + "." + str).c_str());
			/*
			   if (light_handles[str] == -1) {
			   std::cerr << "/!\\ couldn't find " + locstr + "." + str << std::endl;
			   }
			   */
		}

		glUniform3fv(light_handles["position"], 1, glm::value_ptr(light.position));
		glUniform4fv(light_handles["diffuse"], 1, glm::value_ptr(light.diffuse));
		glUniform1f(light_handles["radius"], light.radius);
		glUniform1f(light_handles["intensity"], light.intensity);
	}
}

void engine::sync_spot_lights(const std::vector<uint32_t>& lights) {
	size_t active = min(MAX_LIGHTS, lights.size());
	glUniform1i(glGetUniformLocation(shader.first, "active_spot_lights"), active);

	for (size_t i = 0; i < active; i++) {
		// TODO: check light index
		const auto& light = spot_lights[lights[i]];

		// TODO: cache uniform locations (good idea: add a caching layer to glman)
		// TODO: also updating all these uniforms can't be very efficient for a lot of
		//       moving point lights... maybe look into how uniform buffer objects work
		std::string locstr = "spot_lights[" + std::to_string(i) + "]";
		std::map<std::string, GLint> light_handles;

		for (std::string str : {
				"position", "diffuse", "direction",
				"radius", "intensity", "angle"
		}) {
			light_handles[str] =
				glGetUniformLocation(shader.first, (locstr + "." + str).c_str());
			/*
			   if (light_handles[str] == -1) {
			   std::cerr << "/!\\ couldn't find " + locstr + "." + str << std::endl;
			   }
			   */
		}

		glUniform3fv(light_handles["position"], 1, glm::value_ptr(light.position));
		glUniform4fv(light_handles["diffuse"], 1, glm::value_ptr(light.diffuse));
		glUniform3fv(light_handles["direction"], 1, glm::value_ptr(light.direction));
		glUniform1f(light_handles["radius"], light.radius);
		glUniform1f(light_handles["intensity"], light.intensity);
		glUniform1f(light_handles["angle"], light.angle);
	}
}

void engine::sync_directional_lights(const std::vector<uint32_t>& lights) {
	size_t active = min(MAX_LIGHTS, lights.size());
	glUniform1i(glGetUniformLocation(shader.first, "active_directional_lights"),
		active);

	for (size_t i = 0; i < active; i++) {
		// TODO: check light index
		const auto& light = directional_lights[lights[i]];

		// TODO: cache uniform locations (good idea: add a caching layer to glman)
		// TODO: also updating all these uniforms can't be very efficient for a lot of
		//       moving point lights... maybe look into how uniform buffer objects work
		std::string locstr = "directional_lights[" + std::to_string(i) + "]";
		std::map<std::string, GLint> light_handles;

		for (std::string str : { "position", "diffuse", "direction", "intensity" }) {
			light_handles[str] =
				glGetUniformLocation(shader.first, (locstr + "." + str).c_str());
			/*
			   if (light_handles[str] == -1) {
			   std::cerr << "/!\\ couldn't find " + locstr + "." + str << std::endl;
			   }
			   */
		}

		glUniform3fv(light_handles["position"], 1, glm::value_ptr(light.position));
		glUniform4fv(light_handles["diffuse"], 1, glm::value_ptr(light.diffuse));
		glUniform3fv(light_handles["direction"], 1, glm::value_ptr(light.direction));
		glUniform1f(light_handles["intensity"], light.intensity);
	}
}

void engine::compile_lights(void) {
	active_lights.point.clear();
	active_lights.spot.clear();
	active_lights.directional.clear();

	for (auto& [id, lit] : point_lights) {
		active_lights.point.push_back(id);
	}

	for (auto& [id, lit] : spot_lights) {
		active_lights.spot.push_back(id);
	}

	for (auto& [id, lit] : directional_lights) {
		active_lights.directional.push_back(id);
	}
}

void engine::update_lights(void) {
	compile_lights();
	sync_point_lights(active_lights.point);
	sync_spot_lights(active_lights.spot);
	sync_directional_lights(active_lights.directional);
	DO_ERROR_CHECK();
}

float grendx::light_extent(struct engine::point_light *p, float threshold) {
	if (p) {
		//auto& lit = lights[id];
		return p->radius * (sqrt((p->intensity * p->diffuse.w)/threshold) - 1);
	}

	return 0.0;
}
