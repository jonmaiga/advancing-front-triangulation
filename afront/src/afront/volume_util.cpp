#include "volume_util.h"

namespace playchilla {
bool in_air(double v) {
	return v < 0;
}

bool in_air(const volume* source, const vec3& pos) {
	return in_air(source->get_value(pos));
}

bool in_solid(const volume* source, const vec3& pos) {
	return !in_air(source, pos);
}


struct volume_surface_result {
	double initial_signed_distance;
	vec3 pos;
	double signed_distance;
};

volume_surface_result find_surface_along_ray(const volume* volume,
                                             vec3 pos, const vec3& dir, int max_tries) {
	auto signed_distance = volume->get_value(pos);
	auto last_signed_distance = signed_distance;
	int steps = 0;
	double step_multiplier = 1.;
	volume_surface_result best_result{signed_distance, pos, signed_distance};
	while (steps < max_tries) {
		if (signed_distance * last_signed_distance < 0) {
			step_multiplier *= 0.5;
		}
		pos += dir * signed_distance * step_multiplier;
		last_signed_distance = signed_distance;
		signed_distance = volume->get_value(pos);
		if (std::abs(signed_distance) < std::abs(best_result.signed_distance)) {
			best_result.signed_distance = signed_distance;
			best_result.pos = pos;
		}
		++steps;
	}
	return best_result;
}

std::optional<vec3> find_surface_along_ray(const volume* volume, const vec3& pos, const vec3& dir, double allowed_surface_distance) {
	auto r = find_surface_along_ray(volume, pos, dir, 10);
	if (std::abs(r.signed_distance) <= allowed_surface_distance) {
		return r.pos;
	}
	const auto refined = find_surface_along_ray(volume, r.pos, dir, 20);
	if (std::abs(refined.signed_distance) < std::abs(r.signed_distance)) {
		r.pos = refined.pos;
		r.signed_distance = refined.signed_distance;
	}
	if (std::abs(r.signed_distance) <= allowed_surface_distance) {
		return r.pos;
	}
	return {};
}

std::optional<vec3> find_random_surface_pos(const volume* volume) {
	static mx3::random r(123);
	for (int i = 0; i < 10; ++i) {
		const auto p = vec3d::create_random_dir(r);
		if (const auto& sp = find_surface_along_ray(volume, p, p, 1.0)) {
			return sp;
		}
	}
	return {};
}

std::optional<vec3> follow_surface(const volume* source, const vec3& start_pos, const vec3& ba_dir, double step_size, double distance) {
	vec3 surface_pos = start_pos;
	std::optional<vec3> last_pos;
	const double max_steps = distance / step_size;
	for (int i = 0; i < max_steps; ++i) {
		auto maybe_normal = calc_normal(source, surface_pos, distance);
		if (!maybe_normal) {
			break;
		}

		vec3 dir = ba_dir.cross(*maybe_normal).normalize();
		vec3 test_pos = surface_pos + dir * step_size;
		auto maybe_surface_pos = find_surface(source, test_pos, step_size, distance);
		if (!maybe_surface_pos) {
			break;
		}
		surface_pos = *maybe_surface_pos;
		last_pos = surface_pos;
	}
	return last_pos;
}

std::optional<vec3> find_surface(const volume* source, const vec3& start_pos, double step_size, double distance) {
	if (const auto maybe_surface_dir = calc_normal(source, start_pos, distance)) {
		const vec3& surface_dir = *maybe_surface_dir;
		const bool air = in_air(source->get_value(start_pos));
		return find_surface(source, start_pos, air ? -surface_dir : surface_dir, air, step_size, distance);
	}
	return {};
}

std::optional<vec3> find_surface(const volume* source, const vec3& pos, const vec3& dir, bool pos_in_air, double step_size, double max_distance) {
	for (double s = step_size; s <= max_distance; s += step_size) {
		vec3 test_pos = pos + dir * s;
		if (pos_in_air != in_air(source, test_pos)) {
			return test_pos;
		}
	}
	return {};
}

bool is_blocked(const volume* source, const vec3& from, const vec3& to, double step_size) {
	if (!in_air(source, from)) {
		return true;
	}
	const vec3 dir = (to - from);
	return static_cast<bool>(find_surface(source, from, dir.normalize(), true, step_size, dir.length()));
}

std::optional<vec3> find_solid(const volume* source, const vec3& from, double step_size, double distance) {
	if (in_solid(source, from)) {
		return from;
	}
	return find_surface(source, from, step_size, distance);
}

std::optional<vec3> find_air(const volume* source, const vec3& from, double step_size, double distance) {
	if (in_air(source, from)) {
		return from;
	}
	return find_surface(source, from, step_size, distance);
}

std::optional<vec3> calc_surface_dir(const volume* source, const vec3& pos, double distance) {
	if (const auto& normal = calc_normal(source, pos, distance)) {
		return in_air(source, pos) ? -normal.value() : normal;
	}
	return {};
}

std::optional<vec3> calc_normal(const volume* source, const vec3& pos, double distance) {
	const double d = 0.4 * distance;
	auto normal = vec3(
		source->get_value(pos.x - d, pos.y, pos.z) - source->get_value(pos.x + d, pos.y, pos.z),
		source->get_value(pos.x, pos.y - d, pos.z) - source->get_value(pos.x, pos.y + d, pos.z),
		source->get_value(pos.x, pos.y, pos.z - d) - source->get_value(pos.x, pos.y, pos.z + d));
	if (normal.length_sqr() < EpsilonSqr) {
		return {};
	}
	normal.normalize_self();
	return normal.length_sqr() < EpsilonSqr ? std::optional<vec3>{} : std::optional<vec3>(normal);
}
}
