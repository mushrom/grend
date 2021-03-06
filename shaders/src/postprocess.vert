#define VERTEX_SHADER

precision mediump sampler2D;
precision highp float;

#include <lib/compat.glsl>

IN vec3 v_position;
IN vec2 v_texcoord;
OUT vec2 f_texcoord;

void main(void) {
	gl_Position = vec4(v_position, 1.0);
	f_texcoord = v_texcoord;
}
