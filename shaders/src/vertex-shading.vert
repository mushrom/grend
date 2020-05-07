#define ENABLE_DIFFUSION 1
#define ENABLE_SPECULAR_HIGHLIGHTS 1
#define ENABLE_SKYBOX 1

#include <lib/shading-uniforms.glsl>

attribute vec3 in_Position;
attribute vec2 texcoord;
attribute vec3 in_Color;
attribute vec3 v_normal;
attribute vec3 v_tangent;
attribute vec3 v_bitangent;

varying vec3 ex_Color;
varying vec2 f_texcoord;

void main(void) {
	vec4 v_coord = vec4(in_Position, 1.0);
	// TODO: make this a uniform
	vec3 ambient_light = vec3(0.2);
	vec3 view_dir = normalize(vec3(v_inv * vec4(0, 0, 0, 1) - m * v_coord));

	mat4 mvp = p*v*m;
	//vec3 total_light = ambient_light;
	vec3 total_light = vec3(anmaterial.diffuse.x * ambient_light.x,
	                        anmaterial.diffuse.y * ambient_light.y,
	                        anmaterial.diffuse.z * ambient_light.z);
	vec3 normal_dir = normalize(m_3x3_inv_transp * v_normal);

	for (int i = 0; i < active_lights && i < max_lights; i++) {
		vec3 light_dir;
		float attenuation;

		if (lights[i].position.w == 0.0) {
			attenuation = 1.0;
			light_dir = normalize(vec3(lights[i].position));

		} else {
			vec3 light_vertex = vec3(lights[i].position - m * v_coord);
			float distance = length(light_vertex);
			attenuation = 1.0 / (lights[i].const_attenuation
			                     + lights[i].linear_attenuation * distance
			                     + lights[i].quadratic_attenuation * distance * distance);

			//attenuation = mix(1, attenuation, lights[i].position.w)
			light_dir = normalize(light_vertex);
			//light_dir = normalize(vec3(lights[i].position));
		}

		vec3 diffuse_reflection = vec3(0.0);
		vec3 specular_reflection = vec3(0);
		vec3 env_light = vec3(0);

#if ENABLE_DIFFUSION
		diffuse_reflection =
			// diminish contribution from diffuse lighting for a more cartoony look
			//0.5 *
			attenuation * vec3(lights[i].diffuse) * vec3(anmaterial.diffuse)
			* max(0.0, dot(normal_dir, light_dir));
#endif

#if ENABLE_SPECULAR_HIGHLIGHTS
		if (anmaterial.metalness > 0.1 && dot(normal_dir, light_dir) >= 0.0) {
			specular_reflection = anmaterial.specular.w * attenuation
				* vec3(lights[i].specular)
				* vec3(anmaterial.specular)
				* pow(max(0.0, dot(reflect(-light_dir, normal_dir), view_dir)),
				      anmaterial.metalness);
		}
#endif

		total_light += diffuse_reflection + specular_reflection;
	}

	ex_Color = vec3(total_light); 
	gl_Position = normalize(mvp * vec4(in_Position, 1.0));
	//gl_Position = mvp * vec4(in_Position, 0.25);

	f_texcoord = texcoord;
}