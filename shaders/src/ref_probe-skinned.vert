#define VERTEX_SHADER

precision highp float;
precision mediump sampler2D;
precision mediump samplerCube;

#include <lib/compat.glsl>
#include <lib/shading-varying.glsl>
#include <lib/skinning-uniforms.glsl>

uniform mat4 m, v, p;

void main(void) {
	mat4 skinMatrix =
		a_weights.x * joints[int(a_joints.x)]
		+ a_weights.y * joints[int(a_joints.y)]
		+ a_weights.z * joints[int(a_joints.z)]
		+ a_weights.w * joints[int(a_joints.w)];

	f_normal = normalize(v_normal);
	f_texcoord = texcoord;
	f_position = m*skinMatrix*vec4(in_Position, 1);

	gl_Position = p*v*m * skinMatrix * vec4(in_Position, 1);
}
