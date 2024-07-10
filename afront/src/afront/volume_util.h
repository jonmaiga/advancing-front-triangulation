#pragma once

#include <optional>
#include "volume.h"

namespace playchilla {
std::optional<vec3> find_surface_along_ray(const volume*, const vec3& pos, const vec3& dir, double allowed_surface_distance);
std::optional<vec3> find_random_surface_pos(const volume*);

std::optional<vec3> follow_surface(const volume*, const vec3& start_pos, const vec3& ba_dir, double step_size, double distance);
std::optional<vec3> find_surface(const volume*, const vec3& start_pos, double step_size, double distance);
std::optional<vec3> find_surface(const volume*, const vec3& pos, const vec3& dir, bool pos_in_air, double step_size, double max_distance);

bool is_blocked(const volume*, const vec3& from, const vec3& to, double step_size);
std::optional<vec3> find_solid(const volume*, const vec3& from, double step_size, double distance);
std::optional<vec3> find_air(const volume*, const vec3& from, double step_size, double distance);

std::optional<vec3> calc_surface_dir(const volume*, const vec3& pos, double distance);
std::optional<vec3> calc_normal(const volume*, const vec3& pos, double distance);
}
