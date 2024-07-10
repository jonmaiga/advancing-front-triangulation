#include "game_shader_settings.h"

#include <array>
#include <linmath.h>

#include "shader_program.h"
#include "client/util/relative_camera.h"
#include "client/render/light_view.h"
#include "client/render/mesh_view.h"
#include "core/math/vec3.h"
#include "glad/glad.h"
#include "xgl/xgl.h"

namespace playchilla {namespace {
void to_mat4(mat4x4& d, const matrix4& s) {
	d[0][0] = static_cast<float>(s.m00);
	d[1][0] = static_cast<float>(s.m01);
	d[2][0] = static_cast<float>(s.m02);
	d[3][0] = static_cast<float>(s.m03);
	d[0][1] = static_cast<float>(s.m10);
	d[1][1] = static_cast<float>(s.m11);
	d[2][1] = static_cast<float>(s.m12);
	d[3][1] = static_cast<float>(s.m13);
	d[0][2] = static_cast<float>(s.m20);
	d[1][2] = static_cast<float>(s.m21);
	d[2][2] = static_cast<float>(s.m22);
	d[3][2] = static_cast<float>(s.m23);
	d[0][3] = static_cast<float>(s.m30);
	d[1][3] = static_cast<float>(s.m31);
	d[2][3] = static_cast<float>(s.m32);
	d[3][3] = static_cast<float>(s.m33);
}

void to_camera_relative(const vec3& camera_pos, matrix4& model_world_out) {
	const vec3& model_rel_camera = model_world_out.get_translation() - camera_pos;
	model_world_out.m03 = model_rel_camera.x;
	model_world_out.m13 = model_rel_camera.y;
	model_world_out.m23 = model_rel_camera.z;
}

void glUniformEx(GLint location, const mat4x4& m) {
	gl_check(glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(m)));
}
}}

namespace playchilla {
shader_preprocessor create_define(const std::string& define) {
	return [code = "#define " + define + "\n"]() {
		return code;
	};
}

shader_preprocessor create_version(int version) {
	return [code = "#version " + std::to_string(version) + "\n"]() {
		return code;
	};
}

shader_preprocessor create_extension(const std::string& extension) {
	return [code = "#extension " + extension + ":require\n"]() {
		return code;
	};
}

shader_settings create_point_light(const shader_program& program) {
	static constexpr auto MaxLights = 2;
	std::array<GLint, MaxLights> u_position_location{};
	std::array<GLint, MaxLights> u_diffuse_location{};
	GLint u_point_light_count_location = program.get_uniform_location("u_pointLightCount");
	for (int i = 0; i < MaxLights; ++i) {
		std::string si = "u_pointLights[" + std::to_string(i) + "]";
		u_position_location[i] = program.get_uniform_location(si + ".pos");
		u_diffuse_location[i] = program.get_uniform_location(si + +".diffuse");
	}

	shader_settings settings;
	settings.on_bind_shader = {
		[u_point_light_count_location, u_position_location, u_diffuse_location](const shader_environment& environment, const mesh_view*) {
			const auto& lights = environment.active_light_views;
			const int used_lights = std::min(MaxLights, static_cast<int>(lights.size()));
			gl_check(glUniform1i(u_point_light_count_location, used_lights));
			for (int i = 0; i < used_lights; ++i) {
				const auto* light = lights[i];
				const auto light_pos = light->get_transform().get_pos();
				const auto p = light_pos - environment.active_camera->get_pos();
				const auto c = light->get_diffuse_color();
				gl_check(glUniform3f(u_position_location[i], p.x, p.y, p.z));
				gl_check(glUniform4f(u_diffuse_location[i], c.r, c.g, c.b, c.a));
			}
		}
	};
	return settings;
}

shader_settings create_terrain_mesh(const shader_program& program) {
	GLint a_position = program.get_attrib_location("a_position");
	GLint a_normal = program.get_attrib_location("a_normal");
	GLint a_color = program.get_attrib_location("a_color");
	shader_settings settings;
	settings.bytes_per_vertex = 3 * 4 + 3 * 4 + 4 * 4;
	settings.on_pre_render = {
		[a_position, a_normal, a_color, stride = settings.bytes_per_vertex](const shader_environment&, const mesh_view*) {
			gl_check(glEnableVertexAttribArray(a_position));
			gl_check(glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, stride, nullptr));

			gl_check(glEnableVertexAttribArray(a_normal));
			gl_check(glVertexAttribPointer(a_normal, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * 4)));

			gl_check(glEnableVertexAttribArray(a_color));
			gl_check(glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * 3 * 4)));
		}
	};

	return settings;
}

shader_settings create_entity_mesh(const shader_program& program) {
	GLint a_position = program.get_attrib_location("a_position");
	GLint a_normal = program.get_attrib_location("a_normal");
	shader_settings settings;
	settings.bytes_per_vertex = 4 * 6;
	settings.on_pre_render = {
		[a_position, a_normal, stride = settings.bytes_per_vertex](const shader_environment&, const mesh_view*) {
			gl_check(glEnableVertexAttribArray(a_position));
			gl_check(glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, stride, nullptr));

			gl_check(glEnableVertexAttribArray(a_normal));
			gl_check(glVertexAttribPointer(a_normal, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * 4)));
		}
	};

	return settings;
}

shader_settings create_camera_relative(const shader_program& program) {
	GLint u_projection_view = program.get_uniform_location("u_projectionView");
	GLint u_model_world = program.get_uniform_location("u_modelWorld");
	shader_settings settings;
	settings.on_bind_shader = {
		[u_projection_view](const shader_environment& environment, const mesh_view*) {
			mat4x4 m4;
			to_mat4(m4, environment.active_camera->get_combined());
			glUniformEx(u_projection_view, m4);
		}
	};

	settings.on_pre_render = {
		[u_model_world](const shader_environment& environment, const mesh_view* mesh_view) {
			matrix4 model_world = mesh_view->get_transform().get_matrix();
			to_camera_relative(environment.active_camera->get_pos(), model_world);
			// todo:
			mat4x4 m;
			to_mat4(m, model_world);
			glUniformEx(u_model_world, m);
		}
	};
	return settings;
}

shader_settings create_material_color(const shader_program& program) {
	shader_settings settings;
	GLint u_material_diffuse = program.get_uniform_location("u_materialDiffuse");
	settings.on_pre_render = {
		[u_material_diffuse](const shader_environment&, const mesh_view*) {
			gl_check(glUniform4f(u_material_diffuse, 1.f, 1.f, 1.f, 1.f));
		}
	};
	return settings;
}

shader_settings create_line_settings(const shader_program& program) {
	GLint a_position = program.get_attrib_location("a_position");
	GLint a_color = program.get_attrib_location("a_color");
	shader_settings settings;
	settings.bytes_per_vertex = 3 * 4 + 4 * 4;
	settings.on_pre_render = {
		[a_position, a_color, stride = settings.bytes_per_vertex](const shader_environment&, const mesh_view*) {
			gl_check(glEnableVertexAttribArray(a_position));
			gl_check(glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, stride, nullptr));

			gl_check(glEnableVertexAttribArray(a_color));
			gl_check(glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * 4)));
		}
	};
	return settings;
}

shader_settings create_tone_map(const shader_program& program, double exposure, double darkness) {
	shader_settings settings;
	GLint u_exposure = program.get_uniform_location("u_exposure");
	GLint u_darkness = program.get_uniform_location("u_darkness");
	settings.on_bind_shader = {
		[u_exposure, u_darkness, exposure = static_cast<float>(exposure), darkness = static_cast<float>(darkness)](const shader_environment&, const mesh_view*) {
			gl_check(glUniform1f(u_exposure, exposure));
			gl_check(glUniform1f(u_darkness, darkness));
		}
	};
	return settings;
}

shader_settings create_fresnel(const shader_program& program, double scale, double power) {
	shader_settings settings;
	GLint u_scale = program.get_uniform_location("u_fresnelScale");
	GLint u_power = program.get_uniform_location("u_fresnelPow");
	settings.on_bind_shader = {
		[u_scale, u_power, scale = static_cast<float>(scale), power = static_cast<float>(power)](const shader_environment&, const mesh_view*) {
			gl_check(glUniform1f(u_scale, scale));
			gl_check(glUniform1f(u_power, power));
		}
	};
	return settings;
}

shader_settings create_diffuse_squeeze(const shader_program& program, double squeeze) {
	shader_settings settings;
	GLint u_diffuse = program.get_uniform_location("u_squeezeFrom");
	settings.on_bind_shader = {
		[u_diffuse, squeeze = static_cast<float>(squeeze)](const shader_environment&, const mesh_view*) {
			gl_check(glUniform1f(u_diffuse, squeeze));
		}
	};
	return settings;
}
}
