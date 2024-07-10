#pragma once

#include <functional>
#include <vector>
#include <glad/glad.h>

namespace playchilla {
struct shader_environment;
class mesh_view;

using shader_callback = std::function<void(const shader_environment&, const mesh_view*)>;
using shader_pre_render_callback = std::function<void(const shader_environment&, const mesh_view*)>;

struct shader_settings {
	void append(const shader_settings&);
	void bind_shader(const shader_environment&, const mesh_view*) const;
	void pre_render(const shader_environment&, const mesh_view*) const;

	std::vector<shader_callback> on_bind_shader;
	std::vector<shader_pre_render_callback> on_pre_render;
	GLsizei bytes_per_vertex = {};
};
}
