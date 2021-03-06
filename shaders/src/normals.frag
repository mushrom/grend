#define FRAGMENT_SHADER

precision highp float;
precision mediump sampler2D;
precision mediump samplerCube;

#include <lib/compat.glsl>
#include <lib/shading-uniforms.glsl>
#include <lib/shading-varying.glsl>

void main(void) {
	vec3 normidx = texture2D(normal_map, f_texcoord).rgb;
	vec3 normal_dir = normalize(TBN * normalize(normidx * 2.0 - 1.0));

	FRAG_COLOR  = vec4(normal_dir, 1.0);
}
