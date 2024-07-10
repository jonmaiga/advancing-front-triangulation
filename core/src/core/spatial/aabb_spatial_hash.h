#pragma once

#include "spatial_hash.h"
#include "core/math/aabb.h"

namespace playchilla {
struct aabb_spatial_hash_value {
	aabb bb;
	int64_t cx1 = 0;
	int64_t cy1 = 0;
	int64_t cz1 = 0;
	int64_t cx2 = 0;
	int64_t cy2 = 0;
	int64_t cz2 = 0;
	uint64_t timestamp = 0;

	aabb_spatial_hash_value(const aabb& bb) : bb(bb) {
	}

	bool update(double x1, double y1, double z1, double x2, double y2, double z2) {
		if (x1 == bb.x1() && y1 == bb.y1() && z1 == bb.z1() &&
			x2 == bb.x2() && y2 == bb.y2() && z2 == bb.z2()) {
			return false;
		}
		bb = aabb::create_from_min_max({x1,y1,z1}, {x2,y2,z2});
		return true;
	}

	bool overlaps(const aabb& other) const {
		return bb.overlaps(other);
	}

	bool is_inside(double x, double y, double z) const {
		return bb.is_inside(x, y, z);
	}
};


class aabb_spatial_hash : public spatial_hash<aabb_spatial_hash_value*> {
public:
	aabb_spatial_hash(double cellSize) : spatial_hash(cellSize) {
	}

	void add(aabb_spatial_hash_value* shv) {
		const auto& min = shv->bb.get_min() * _inv_cell_size;
		const auto& max = shv->bb.get_max() * _inv_cell_size;
		shv->cx1 = floor_to<int64_t>(min.x);
		shv->cy1 = floor_to<int64_t>(min.y);
		shv->cz1 = floor_to<int64_t>(min.z);
		shv->cx2 = floor_to<int64_t>(max.x);
		shv->cy2 = floor_to<int64_t>(max.y);
		shv->cz2 = floor_to<int64_t>(max.z);

		for (int64_t cz = shv->cz1; cz <= shv->cz2; ++cz) {
			for (int64_t cy = shv->cy1; cy <= shv->cy2; ++cy) {
				for (int64_t cx = shv->cx1; cx <= shv->cx2; ++cx) {
					add_to_cell(shv, cx, cy, cz);
				}
			}
		}
	}

	void remove(aabb_spatial_hash_value* shv) {
		for (int64_t cz = shv->cz1; cz <= shv->cz2; ++cz) {
			for (int64_t cy = shv->cy1; cy <= shv->cy2; ++cy) {
				for (int64_t cx = shv->cx1; cx <= shv->cx2; ++cx) {
					remove_from_cell(shv, cx, cy, cz);
				}
			}
		}
	}

	void update(aabb_spatial_hash_value* shv) {
		const auto& min = shv->bb.get_min() * _inv_cell_size;
		const auto& max = shv->bb.get_max() * _inv_cell_size;
		const auto new_cx1 = floor_to<int64_t>(min.x);
		const auto new_cy1 = floor_to<int64_t>(min.y);
		const auto new_cz1 = floor_to<int64_t>(min.z);
		const auto new_cx2 = floor_to<int64_t>(max.x);
		const auto new_cy2 = floor_to<int64_t>(max.y);
		const auto new_cz2 = floor_to<int64_t>(max.z);

		// add new
		for (int64_t cz = new_cz1; cz <= new_cz2; ++cz) {
			for (int64_t cy = new_cy1; cy <= new_cy2; ++cy) {
				for (int64_t cx = new_cx1; cx <= new_cx2; ++cx) {
					if (cx < shv->cx1 || cx > shv->cx2 ||
						cy < shv->cy1 || cy > shv->cy2 ||
						cz < shv->cz1 || cz > shv->cz2) {
						add_to_cell(shv, cx, cy, cz);
					}
				}
			}
		}

		// remove old
		for (int64_t cz = shv->cz1; cz <= shv->cz2; ++cz) {
			for (int64_t cy = shv->cy1; cy <= shv->cy2; ++cy) {
				for (int64_t cx = shv->cx1; cx <= shv->cx2; ++cx) {
					if (cx < new_cx1 || cx > new_cx2 ||
						cy < new_cy1 || cy > new_cy2 ||
						cz < new_cz1 || cz > new_cz2) {
						remove_from_cell(shv, cx, cy, cz);
					}
				}
			}
		}

		shv->cx1 = new_cx1;
		shv->cy1 = new_cy1;
		shv->cz1 = new_cz1;
		shv->cx2 = new_cx2;
		shv->cy2 = new_cy2;
		shv->cz2 = new_cz2;
	}

	std::vector<aabb_spatial_hash_value*> get_values(double x, double y, double z) const {
		std::vector<aabb_spatial_hash_value*> collect;
		for (const auto& v : get_cell_values(x, y, z)) {
			if (v->is_inside(x, y, z)) {
				collect.push_back(v);
			}
		}
		return collect;
	}

	std::vector<aabb_spatial_hash_value*> get_values(const aabb& bb) const {
		++_timestamp;
		const auto& min = bb.get_min() * _inv_cell_size;
		const auto& max = bb.get_max() * _inv_cell_size;
		const auto cx1 = floor_to<int64_t>(min.x);
		const auto cy1 = floor_to<int64_t>(min.y);
		const auto cz1 = floor_to<int64_t>(min.z);
		const auto cx2 = floor_to<int64_t>(max.x);
		const auto cy2 = floor_to<int64_t>(max.y);
		const auto cz2 = floor_to<int64_t>(max.z);

		std::vector<aabb_spatial_hash_value*> collect;
		for (int64_t cz = cz1; cz <= cz2; ++cz) {
			for (int64_t cy = cy1; cy <= cy2; ++cy) {
				for (int64_t cx = cx1; cx <= cx2; ++cx) {
					for (aabb_spatial_hash_value* v : get_cell_values(cx, cy, cz)) {
						if (v->timestamp >= _timestamp) {
							continue;
						}
						v->timestamp = _timestamp;
						if (v->overlaps(bb)) {
							collect.push_back(v);
						}
					}
				}
			}
		}
		return collect;
	}

private:
	mutable uint64_t _timestamp = 1;
};
}
