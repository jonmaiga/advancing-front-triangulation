#include "game_shaders.h"

#include <vector>

#include "game_shader_settings.h"
#include "core/debug/assertion.h"
#include "core/debug/log.h"
#include "core/util/file_util.h"


namespace {
using namespace playchilla;

std::string preprocess(const std::string& code, const std::vector<shader_preprocessor>& preprocessors) {
	std::string inject;
	for (const shader_preprocessor& setting : preprocessors) {
		inject += setting();
	}
	return inject + "\n" + code;
}

auto get_terrain_preprocessors() {
	return std::vector{
		create_version(120),
		create_define("COLOR_ATTRIBUTE"),
		create_define("NORMAL_ATTRIBUTE"),
		create_define("POINT_LIGHT"),
		create_define("TONE_MAP"),
		create_define("DIFFUSE_NORMAL_NOISE"),
		create_define("FRESNEL"),
	};
}

auto get_terrain_settings(const shader_program* program) {
	shader_settings settings;
	settings.append(create_camera_relative(*program));
	settings.append(create_point_light(*program));
	settings.append(create_terrain_mesh(*program));
	settings.append(create_tone_map(*program, 1.3, 0.5));
	settings.append(create_fresnel(*program, 0.1, 1.4));
	return settings;
}


shader create_terrain_shader() {
	const auto preprocessors = get_terrain_preprocessors();

	shader shader;
	shader.program = std::make_unique<shader_program>(
		preprocess(file::read_as_string_must_exist("data/shader/world_vs.glsl"), preprocessors),
		preprocess(file::read_as_string_must_exist("data/shader/world_fs.glsl"), preprocessors));

	if (!shader.program->validate()) {
		logger() << shader.program->get_program_log() << "\n";
		assertion(false, "Shader did not validate");
	}
	shader.settings = get_terrain_settings(shader.program.get());
	return shader;
}

shader create_sun_shader() {
	auto preprocessors = get_terrain_preprocessors();
	preprocessors.push_back(create_define("LIGHT_INVERT_NORMAL"));

	shader shader;
	shader.program = std::make_unique<shader_program>(
		preprocess(file::read_as_string_must_exist("data/shader/world_vs.glsl"), preprocessors),
		preprocess(file::read_as_string_must_exist("data/shader/world_fs.glsl"), preprocessors));

	if (!shader.program->validate()) {
		logger() << shader.program->get_program_log() << "\n";
		assertion(false, "Shader did not validate");
	}
	shader.settings = get_terrain_settings(shader.program.get());
	return shader;
}


shader create_entity_shader() {
	const auto preprocessors = std::vector{
		create_version(120),
		create_define("NORMAL_ATTRIBUTE"),
		create_define("POINT_LIGHT")
	};
	shader shader;
	shader.program = std::make_unique<shader_program>(
		preprocess(file::read_as_string_must_exist("data/shader/world_vs.glsl"), preprocessors),
		preprocess(file::read_as_string_must_exist("data/shader/world_fs.glsl"), preprocessors));

	if (!shader.program->validate()) {
		logger() << shader.program->get_program_log() << "\n";
		assertion(false, "Shader did not validate");
	}
	shader.settings.append(create_camera_relative(*shader.program));
	shader.settings.append(create_point_light(*shader.program));
	shader.settings.append(create_entity_mesh(*shader.program));
	shader.settings.append(create_material_color(*shader.program));
	return shader;
}

shader create_line_shader() {
	const auto preprocessors = {
		create_version(120)
	};

	shader shader;
	shader.program = std::make_unique<shader_program>(
		preprocess(file::read_as_string_must_exist("data/shader/line_vs.glsl"), preprocessors),
		preprocess(file::read_as_string_must_exist("data/shader/line_fs.glsl"), preprocessors)
	);
	if (!shader.program->validate()) {
		logger() << shader.program->get_program_log() << "\n";
		assertion(false, "Shader did not validate");
	}
	shader.settings.append(create_line_settings(*shader.program));
	shader.settings.append(create_camera_relative(*shader.program));
	return shader;
}
}

namespace playchilla {
shader_map create_shaders() {
	shader_map shaders;
	shaders.add(shader_type::sun_shader, create_sun_shader());
	shaders.add(shader_type::terrain_shader, create_terrain_shader());
	shaders.add(shader_type::entity_shader, create_entity_shader());
	shaders.add(shader_type::line_shader, create_line_shader());
	return shaders;
}
}
