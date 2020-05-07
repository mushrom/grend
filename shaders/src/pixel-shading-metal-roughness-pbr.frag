precision highp float;
precision mediump sampler2D;
precision mediump samplerCube;

#define PI 3.1415926

#define ENABLE_DIFFUSION 1
#define ENABLE_SPECULAR_HIGHLIGHTS 1
#define ENABLE_SKYBOX 1
#define ENABLE_REFRACTION 1

#include <lib/shading-uniforms.glsl>
#include <lib/shading-varying.glsl>
#include <lib/attenuation.glsl>
#include <lighting/metal-roughness-pbr.glsl>

void main(void) {
	vec3 normidx = texture2D(normal_map, f_texcoord).rgb;
	vec3 ambient_light = vec3(0.1);
	// TODO: normal mapping still borked
	vec3 normal_dir = normalize(TBN * normalize(normidx * 2.0 - 1.0));

	vec3 view_dir = normalize(vec3(v_inv * vec4(0, 0, 0, 1) - f_position));
	mat4 mvp = p*v*m;

	vec3 metal_roughness_idx = texture2D(specular_map, f_texcoord).rgb;

	vec3 albedo = texture2D(diffuse_map, f_texcoord).rgb;
	float metallic = anmaterial.metalness * metal_roughness_idx.b;
	float roughness = anmaterial.roughness * metal_roughness_idx.g;
	float aoidx = pow(texture2D(ambient_occ_map, f_texcoord).r, 2.0);
	//float aoidx = texture2D(ambient_occ_map, f_texcoord).r;

	vec3 total_light = vec3(0);

	for (int i = 0; i < active_lights && i < max_lights; i++) {
		float atten = attenuation(i, f_position);
		vec3 lum = mrp_lighting(i, mvp, f_position, view_dir,
		                        albedo, normal_dir, metallic, roughness);

		total_light += aoidx*atten*lum;
	}

	// TODO: some kind of curve
	vec3 env = vec3(textureLod(skytexture, reflect(-view_dir, normal_dir),
	                           8.0*roughness));
	//vec3 env = vec3(textureCube(skytexture, reflect(-view_dir, normal_dir)));
	//total_light += env * (0.15*metallic * (1.0 - roughness));
	total_light += env * 0.15*metallic*(1.0-roughness*0.5);

#if ENABLE_REFRACTION
	vec3 ref_light = vec3(0);

	if (anmaterial.opacity < 1.0) {
		ref_light = anmaterial.diffuse.xyz
			* 0.5*vec3(textureCube(skytexture,
			                       refract(-view_dir, normal_dir, 1.0/1.5)));
	}

	total_light += ref_light;
#endif

	vec4 dispnorm = vec4((normal_dir + 1.0)/2.0, 1.0);
	vec4 displight = vec4(total_light, 1.0);
	gl_FragColor = displight;
	//gl_FragColor = dispnorm;
}