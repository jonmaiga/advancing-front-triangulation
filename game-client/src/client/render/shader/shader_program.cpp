#include "shader_program.h"

#include <vector>

#include "core/debug/assertion.h"
#include "core/debug/log.h"
#include "xgl/xgl.h"

namespace playchilla {
shader_program::shader_program(const std::string& vertex_code, const std::string& fragment_code) {
	const char* vc = vertex_code.c_str();
	_vertex_id = gl_check_zero(glCreateShader(GL_VERTEX_SHADER));
	gl_check(glShaderSource(_vertex_id, 1, &vc, nullptr));
	gl_check(glCompileShader(_vertex_id));

	const char* fc = fragment_code.c_str();
	_fragment_id = gl_check_zero(glCreateShader(GL_FRAGMENT_SHADER));
	gl_check(glShaderSource(_fragment_id, 1, &fc, nullptr));
	gl_check(glCompileShader(_fragment_id));

	_program_id = gl_check_zero(glCreateProgram());
	gl_check(glAttachShader(_program_id, _vertex_id));
	gl_check(glAttachShader(_program_id, _fragment_id));
	gl_check(glLinkProgram(_program_id));
	if (!_get_get_bool_status(GL_LINK_STATUS)) {
		logger() << "Link error " << get_program_log() << "\n";
		assertion(false, "Shader did not compile");
	}
}

shader_program::~shader_program() {
	gl_check(glUseProgram(0));
	gl_check(glDetachShader(_program_id, _vertex_id));
	gl_check(glDetachShader(_program_id, _fragment_id));
	gl_check(glDeleteShader(_vertex_id));
	gl_check(glDeleteShader(_fragment_id));
	gl_check(glDeleteProgram(_program_id));
}

bool shader_program::validate() const {
	gl_check(glValidateProgram(_program_id));
	return _get_get_bool_status(GL_VALIDATE_STATUS);
}

GLuint shader_program::get_id() const {
	return _program_id;
}

GLint shader_program::get_attrib_location(const std::string& attribute_name) const {
	return gl_check_neg(glGetAttribLocation(_program_id, attribute_name.c_str()));
}

GLint shader_program::get_uniform_location(const std::string& uniform_name) const {
	return gl_check_neg(glGetUniformLocation(_program_id, uniform_name.c_str()));
}

std::string shader_program::get_program_log() const {
	GLint max_length = 0;
	gl_check(glGetProgramiv(_program_id, GL_INFO_LOG_LENGTH, &max_length));
	if (max_length == 0) {
		return "";
	}
	std::vector<GLchar> info_log(max_length);
	gl_check(glGetProgramInfoLog(_program_id, max_length, &max_length, info_log.data()));
	return {info_log.begin(), info_log.end()};
}

bool shader_program::_get_get_bool_status(GLenum status_flag) const {
	GLint status{};
	gl_check(glGetProgramiv(_program_id, status_flag, &status));
	return status == GL_TRUE;
}
}
