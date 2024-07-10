#pragma once

#include <map>
#include <memory>

#include "shader_program.h"
#include "shader_setting.h"

namespace playchilla {
enum class shader_type {
	line_shader,
	entity_shader,
	terrain_shader,
	sun_shader
};

struct shader {
	std::unique_ptr<shader_program> program;
	shader_settings settings;
};

class shader_map {
public:
	void add(shader_type type, shader shader) {
		_programs[type] = std::move(shader);
	}

	const shader* get(shader_type type) const {
		return &_programs.find(type)->second;
	}

	const shader_settings& get_settings(shader_type type) const {
		return get(type)->settings;
	}

private:
	std::map<shader_type, shader> _programs;
};
}
