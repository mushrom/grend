#include <grend/gl_manager.hpp>
#include <grend/glm-includes.hpp>

#include <string>
#include <vector>
#include <map>

#include <fstream>
#include <iostream>
#include <sstream>

namespace grendx {

Shader::Shader(GLuint o, std::string p)
	: Obj(o, Obj::type::Shader), filepath(p)
{
	if (!p.empty()) {
		load(p);
	}
}

// TODO: header for general utility functions (see also split_string())
static std::string load_file(const std::string filename) {
	std::ifstream ifs(filename);
	std::string content((std::istreambuf_iterator<char>(ifs)),
	                    (std::istreambuf_iterator<char>()));

	return content;
}

bool Shader::load(std::string filename) {
	std::cerr << __func__ << ": " << __LINE__ << ": loading "
	          << filename << std::endl;

	std::string source = load_file(filename);
	const char *temp = source.c_str();
	int compiled;

	glShaderSource(obj, 1, (const GLchar**)&temp, 0);
	DO_ERROR_CHECK();
	glCompileShader(obj);
	glGetShaderiv(obj, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		int max_length;
		char *shader_log;

		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &max_length);
		shader_log = new char[max_length];
		glGetShaderInfoLog(obj, max_length, &max_length, shader_log);

		std::string err = (filename + ": " + shader_log);
		std::cerr << err << std::endl;
	}

	filepath = filename;
	return compiled;
}

bool Shader::reload(void) {
	if (!filepath.empty()) {
		return load(filepath);
	}

	return false;
}

Program::ptr load_program(std::string vert, std::string frag) {
	Program::ptr prog = gen_program();

	prog->vertex = gen_shader(GL_VERTEX_SHADER);
	prog->fragment = gen_shader(GL_FRAGMENT_SHADER);

	prog->vertex->load(vert);
	prog->fragment->load(frag);

	glAttachShader(prog->obj, prog->vertex->obj);
	glAttachShader(prog->obj, prog->fragment->obj);
	DO_ERROR_CHECK();

	return prog;
}

bool Program::link(void) {
	glLinkProgram(obj);
	glGetProgramiv(obj, GL_LINK_STATUS, &linked);

	if (!linked) {
		std::string err = (std::string)"error linking program: " + log();
		std::cerr << err << std::endl;
	}

	return linked;
}

std::string Program::log(void) {
	int max_length;
	char *prog_log;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &max_length);
	prog_log = new char[max_length];
	glGetProgramInfoLog(obj, max_length, &max_length, prog_log);

	std::string err = std::string(prog_log);
	delete prog_log;

	return err;
}

bool Program::reload(void) {
	if (vertex && fragment) {
		if (vertex->reload() && fragment->reload()) {
			for (auto& [attr, location] : attributes) {
				glBindAttribLocation(obj, location, attr.c_str());
			}

			return link();
		}
	}

	return false;
}

void Program::attribute(std::string attr, GLuint location) {
	attributes[attr] = location;
	glBindAttribLocation(obj, location, attr.c_str());
}

#define LOOKUP(U) \
	GLint u = lookup(uniform); \
	if (u < 0) return;

void Program::set(std::string uniform, GLint i) {
	LOOKUP(uniform);
	glUniform1i(u, i);
}

void Program::set(std::string uniform, GLfloat f) {
	LOOKUP(uniform);
	glUniform1f(u, f);
}

void Program::set(std::string uniform, glm::vec2 v2) {
	LOOKUP(uniform);
	glUniform2fv(u, 1, glm::value_ptr(v2));
}

void Program::set(std::string uniform, glm::vec3 v3) {
	LOOKUP(uniform);
	glUniform3fv(u, 1, glm::value_ptr(v3));
}

void Program::set(std::string uniform, glm::vec4 v4) {
	LOOKUP(uniform);
	glUniform4fv(u, 1, glm::value_ptr(v4));
}

void Program::set(std::string uniform, glm::mat3 m3) {
	LOOKUP(uniform);
	glUniformMatrix3fv(u, 1, GL_FALSE, glm::value_ptr(m3));
}

void Program::set(std::string uniform, glm::mat4 m4) {
	LOOKUP(uniform);
	glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(m4));
}

GLint Program::lookup(std::string uniform) {
	auto it = uniforms.find(uniform);

	if (it != uniforms.end()) {
		return it->second;
	}

	return uniforms[uniform] = glGetUniformLocation(obj, uniform.c_str());
}

bool Program::cached(std::string uniform) {
	return false;
}

// namespace grendx
}