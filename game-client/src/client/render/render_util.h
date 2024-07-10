#pragma once

#include "shader/game_shader_settings.h"

namespace playchilla {
inline void render_meshes(const shader_environment& environment, const std::vector<const mesh_view*>& mesh_views) {
	render_setting render_settings;
	setup_gl_bindings(render_settings);
	for (const auto* mv : mesh_views) {
		const auto* shader = environment.shader_map->get(mv->get_shader_type());
		auto render_setting = mv->get_render_settings();
		render_setting.program_id = shader->program->get_id();

		const auto bind_result = render_settings.bind(render_setting);

		if (bind_result.shader_program_bound) {
			shader->settings.bind_shader(environment, mv);
		}
		if (bind_result.buffer_bound) {
			mv->get_vbo()->try_upload();
		}
		shader->settings.pre_render(environment, mv);

		GLsizei draw_count = static_cast<GLsizei>(mv->get_vbo()->get_buffer_byte_size()) / shader->settings.bytes_per_vertex;
		gl_check(glDrawArrays(
			render_settings.draw_mode,
			render_settings.draw_offset,
			draw_count));
	}
	gl_check(glUseProgram(0));
}
}
