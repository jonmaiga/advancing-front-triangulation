#pragma once

#include "client/volume/csg.h"

namespace playchilla::test {
inline const volume* create_noisy_planet(csg& csg, double size) {
	const auto r = size * 0.5;
	auto* body = csg.sphere(r).get();
	auto* plains = csg.add_values({body, csg.noise_seed(123).mul_value(0.005)}).get();
	auto* hills = csg.add_values({body, csg.noise_seed(123).fbm().mul_value(0.3)}).get();

	auto* mountain_grounds = csg.noise_seed(123, r).fbm().mul_value(0.1 * r).get();
	auto* mountain_peaks = csg.noise_seed(123, 0.35 * r).fbm().to_range(0, 1).mul_value(0.6 * r).get();
	auto* mountains = csg.add_values({body, mountain_grounds, mountain_peaks}).get();

	auto* lowlands = csg.select(plains, hills, csg.noise_seed(123, 0.5 * r), select_greater(0, 0.3)).get();
	auto* highlands = csg.select(hills, mountains, csg.noise_seed(123, 0.6 * r), select_greater(0, 0.1)).get();
	auto* continent = csg.select(lowlands, highlands, csg.noise_seed(123, 0.8 * r), select_greater(0, 0.1)).get();
	return continent;
}

inline const volume* create_sphere_tunnel(csg& csg, double size) {
	const auto* noisy_sphere = csg.add_values({csg.sphere(size), csg.noise_seed(123, 4)}).get();
	const auto* cube_tunnel = csg.cube({2 * size, 0.2 * size, 0.2 * size}).get();
	const auto* body = csg.differences(noisy_sphere, {cube_tunnel}).get();
	return body;
}
}
