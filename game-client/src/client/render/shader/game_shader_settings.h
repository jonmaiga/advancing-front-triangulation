#pragma once

#include <string>

#include "shader_setting.h"

namespace playchilla {
class shader_program;

struct shader_environment {
	const class relative_camera* active_camera{};
	std::vector<class light_view*> active_light_views;
	const class shader_map* shader_map;
};

using shader_preprocessor = std::function<std::string()>;
shader_preprocessor create_define(const std::string& define);
shader_preprocessor create_version(int version);
shader_preprocessor create_extension(const std::string& extension);

shader_settings create_point_light(const shader_program&);
shader_settings create_terrain_mesh(const shader_program&);
shader_settings create_entity_mesh(const shader_program&);
shader_settings create_camera_relative(const shader_program&);
shader_settings create_material_color(const shader_program&);
shader_settings create_line_settings(const shader_program&);
shader_settings create_tone_map(const shader_program&, double exposure, double darkness);
shader_settings create_fresnel(const shader_program&, double scale, double power);
shader_settings create_diffuse_squeeze(const shader_program&, double squeeze);
}
