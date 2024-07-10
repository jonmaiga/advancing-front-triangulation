#pragma once

#include <string>
#include <glad/glad.h>

namespace playchilla {
class shader_program {
public:
	shader_program(const std::string& vertex_code, const std::string& fragment_code);
	shader_program(const shader_program&) = delete;
	shader_program(shader_program&&) = delete;
	shader_program& operator=(const shader_program&) = delete;
	shader_program& operator=(shader_program&&) = delete;
	~shader_program();

	bool validate() const;

	GLuint get_id() const;

	GLint get_attrib_location(const std::string& attribute_name) const;

	GLint get_uniform_location(const std::string& uniform_name) const;

	std::string get_program_log() const;

private:
	bool _get_get_bool_status(GLenum status_flag) const;

	GLuint _program_id;
	GLuint _vertex_id;
	GLuint _fragment_id;
};
}
