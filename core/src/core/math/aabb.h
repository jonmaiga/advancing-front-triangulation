#pragma once

#include "vec3.h"
#include "core/debug/assertion.h"

namespace playchilla {
class aabb {
public:
	static aabb create_from_min_max(const vec3& min, const vec3& max) {
		assertion(min.is_valid(), "Non valid min vec for aabb");
		assertion(max.is_valid(), "Non valid max vec for aabb");
		vec3 size = max - min;
		return {min + size.scale(0.5), size};
	}

	aabb() = default;

	aabb(const vec3& size) : _half_extents(size * .5) {
	}

	aabb(const vec3& center, const vec3& size) : _center(center), _half_extents(size * .5) {
	}

	aabb(const vec3& center, double half_extent) : 
		_center(center), 
		_half_extents(half_extent, half_extent, half_extent) {
	}

	const vec3& get_center() const {
		return _center;
	}

	vec3 get_size() const { return _half_extents * 2.; }
	vec3 get_min() const { return _center - _half_extents; }
	vec3 get_max() const { return _center + _half_extents; }

	double get_width() const { return 2. * _half_extents.x; }
	double get_height() const { return 2. * _half_extents.y; }
	double get_depth() const { return 2. * _half_extents.z; }

	double x1() const { return _center.x - _half_extents.x; }
	double y1() const { return _center.y - _half_extents.y; }
	double z1() const { return _center.z - _half_extents.z; }
	double x2() const { return _center.x + _half_extents.x; }
	double y2() const { return _center.y + _half_extents.y; }
	double z2() const { return _center.z + _half_extents.z; }

	double get_max_component() const {
		auto size = get_size();
		return max(size.x, max(size.y, size.z));
	}

	aabb& set_scale(double scale) {
		_half_extents *= scale;
		return *this;
	}

	bool is_inside(const vec3& pos) const {
		return is_inside(pos.x, pos.y, pos.z);
	}

	bool is_inside(double x, double y, double z) const {
		const auto& min = get_min();
		const auto& max = get_max();
		return
			x >= min.x && x <= max.x &&
			y >= min.y && y <= max.y &&
			z >= min.z && z <= max.z;
	}

	bool overlaps(const aabb& b) const {
		const auto& min1 = get_min();
		const auto& max1 = get_max();		
		const auto& min2 = b.get_min();
		const auto& max2 = b.get_max();		
		return
			min1.x <= max2.x && max1.x >= min2.x &&
			min1.y <= max2.y && max1.y >= min2.y &&
			min1.z <= max2.z && max1.z >= min2.z;
	}

	aabb& set_position(const vec3& center) {
		_center = center;
		return *this;
	}

private:
	vec3 _center;
	vec3 _half_extents;
};
}
