#define VERTEX_SHADER

precision highp float;
precision mediump sampler2D;
precision mediump samplerCube;

#include <lib/compat.glsl>
#include <lib/shading-varying.glsl>

uniform mat4 m, v, p;

void main(void) {
	f_normal = v_normal;
	f_tangent = v_tangent;
	f_bitangent = vec4(cross(v_normal, v_tangent.xyz) * v_tangent.w, v_tangent.w);

	vec3 T = normalize(vec3(m * f_tangent));
	vec3 B = normalize(vec3(m * f_bitangent));
	vec3 N = normalize(vec3(m * vec4(f_normal, 0)));

	TBN = mat3(T, B, N);

	f_position = m * vec4(in_Position, 1);
	f_texcoord = texcoord;
	f_lightmap = a_lightmap;
	f_color = v_color;

	gl_Position = p*v * f_position;
}
