#include "shader_setting.h"

#include "core/debug/assertion.h"

namespace playchilla {
void shader_settings::append(const shader_settings& settings) {
	on_bind_shader.insert(on_bind_shader.end(), settings.on_bind_shader.begin(), settings.on_bind_shader.end());
	on_pre_render.insert(on_pre_render.end(), settings.on_pre_render.begin(), settings.on_pre_render.end());
	if (settings.bytes_per_vertex > 0) {
		assertion(bytes_per_vertex == 0, "Expected 0 bytes per vertex");
		bytes_per_vertex = settings.bytes_per_vertex;
	}
}

void shader_settings::bind_shader(const shader_environment& environment, const mesh_view* mesh) const {
	for (const auto& bind_shader : on_bind_shader) {
		bind_shader(environment, mesh);
	}
}

void shader_settings::pre_render(const shader_environment& environment, const mesh_view* mesh) const {
	for (const auto& pre_render : on_pre_render) {
		pre_render(environment, mesh);
	}
}
}
