#pragma once

#include <optional>

#include "spatial_hash.h"

namespace playchilla {
template <typename T>
class point_spatial_hash3 : public spatial_hash<T> {
public:
	using spatial_hash<T>::spatial_hash;

	T add(const T& shv) {
		spatial_hash<T>::add_to_cell(shv, get_pos(shv));
		return shv;
	}

	T remove(const T& shv) {
		spatial_hash<T>::remove_from_cell(shv, get_pos(shv));
		return shv;
	}

	bool has_value(const vec3& pos, double r) const {
		bool found_value = false;
		for_each_value_within(pos, r, [&found_value](const T&) {
			found_value = true;
			return false;
		});
		return found_value;
	}

	std::vector<T> get_values(const vec3& pos, double r) const {
		std::vector<T> found;
		for_each_value_within(pos, r, [&found](const T& v) {
			found.push_back(v);
			return true;
		});
		return found;
	}

	template<typename CallbackT>
	void for_each_value_within(const vec3& pos, double r, const CallbackT& callback) const {
		const double inv_cell_size = this->_inv_cell_size;
		const auto cx1 = floor_to<int64_t>((pos.x - r) * inv_cell_size);
		const auto cy1 = floor_to<int64_t>((pos.y - r) * inv_cell_size);
		const auto cz1 = floor_to<int64_t>((pos.z - r) * inv_cell_size);
		const auto cx2 = floor_to<int64_t>((pos.x + r) * inv_cell_size);
		const auto cy2 = floor_to<int64_t>((pos.y + r) * inv_cell_size);
		const auto cz2 = floor_to<int64_t>((pos.z + r) * inv_cell_size);

		const double r2 = r * r;
		for (int64_t cz = cz1; cz <= cz2; ++cz) {
			for (int64_t cy = cy1; cy <= cy2; ++cy) {
				for (int64_t cx = cx1; cx <= cx2; ++cx) {
					for (const T& v : spatial_hash<T>::get_cell_values(cx, cy, cz)) {
						if (pos.distance_sqr(get_pos(v)) <= r2) {
							if (!callback(v)) {
								return;
							}
						}
					}
				}
			}
		}
	}
};
}
