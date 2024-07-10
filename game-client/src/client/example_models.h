#pragma once

namespace playchilla {
inline csg_instance create_bumps(csg& csg, double period, double height_scale) {
	return csg.noise(period).to_range(0, 1).mul_value(period * height_scale);
}

inline csg_instance create_plains(csg& csg, double r) {
	const auto& mr = get_material_registry();
	const double period = 0.01 * r;
	return create_bumps(csg, period, .1).material(mr.plains());
}

inline csg_instance create_hills(csg& csg, double r) {
	const auto& mr = get_material_registry();
	const double period = std::min(1000., r * 0.1);
	return create_bumps(csg, period, 1.5).material(mr.grass());
}

inline csg_instance create_canyons(csg& csg, double r) {
	const auto& mr = get_material_registry();
	const auto depth = std::clamp(0.001 * r, 5., 1000.);
	return csg.constant(-depth).material(mr.rock());
}

inline csg_instance create_mountains(csg& csg, double r, double height_scale) {
	const auto& mr = get_material_registry();
	const double period = 0.1 * r;
	return csg.noise(period).fbm().to_range(0, 1).mul_value(period * height_scale).material(mr.rock());
}

inline csg_instance create_patch(csg& csg, double r, double patch_size, const volume* a, const volume* b) {
	return csg.select(a, b, csg.noise2d(r, 0.1 * patch_size), select_greater(0, 0.01));
}

inline const volume* create_planet(csg& csg, double size) {
	const auto& mr = get_material_registry();
	const double r = 0.5 * size;
	auto* body = csg.sphere(r).get();
	auto* plains = csg.add_values({body, create_plains(csg, r)}).get();
	auto* hills = csg.add_values({body, create_hills(csg, r)}).get();
	auto* canyons = csg.add_values({body, create_canyons(csg, r)}).get();
	auto* mountains = csg.add_values({body, create_mountains(csg, r, 2.)}).get();

	const double patch_size = std::max(80., 0.01 * r);
	auto* canyon_patch = create_patch(csg, r, patch_size, plains, canyons).get();
	auto* lowlands = csg.select(plains, canyon_patch, csg.noise2d(r, patch_size), select_greater(0.5, 0.01)).get();
	auto* highlands = csg.select(hills, mountains, csg.noise(0.6 * r), select_greater(0, 0.1)).get();
	auto* continent = csg.select(lowlands, highlands, csg.noise(0.8 * r), select_greater(0, 0.1)).get();

	return csg(continent).data_type<game_volume_data>({mr.unset()}).get();
}
}
