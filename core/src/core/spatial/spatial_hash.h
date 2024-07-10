#pragma once

#include <unordered_map>
#include <vector>

#include "core/debug/assertion.h"
#include "core/math/vec3.h"
#include "core/util/hash_util.h"

namespace playchilla {

template <typename T>
class spatial_hash {
public:
	spatial_hash(double cell_size) : _inv_cell_size(1. / cell_size), _cell_size(cell_size) {
	}

	double get_cell_size() const {
		return _cell_size;
	}

	std::size_t get_cell_count() const {
		return _map.size();
	}

	uint64_t get_value_count() const {
		return _value_count;
	}

	vec3 get_cell_center(const vec3& pos) const {
		const int64_t x1 = floor_to<int64_t>(pos.x * _inv_cell_size);
		const int64_t y1 = floor_to<int64_t>(pos.y * _inv_cell_size);
		const int64_t z1 = floor_to<int64_t>(pos.z * _inv_cell_size);
		return vec3(x1 + 0.5, y1 + 0.5, z1 + 0.5) * _cell_size;
	}

	/**
	 * Add to cell
	 */
	void add_to_cell(const T& shv, const vec3& pos) {
		_add_to_cell(shv, _hash(pos));
	}

	void add_to_cell(const T& shv, int64_t cx, int64_t cy, int64_t cz) {
		_add_to_cell(shv, _hash(cx, cy, cz));
	}

	/**
	 * Remove from cell
	 */
	void remove_from_cell(const T& shv, const vec3& pos) {
		return _remove_from_cell(shv, _hash(pos));
	}

	void remove_from_cell(const T& shv, int64_t cx, int64_t cy, int64_t cz) {
		_remove_from_cell(shv, _hash(cx, cy, cz));
	}

	void remove_cell(int64_t cx, int64_t cy, int64_t cz) {
		_map.remove(_hash(cx, cy, cz));
	}

	/**
	 * Get cell values
	 */
	const std::vector<T>& get_cell_values(const vec3& pos) const {
		return get_cell_values(_hash(pos));
	}

	const std::vector<T>& get_cell_values(double x, double y, double z) const {
		return get_cell_values(_hash(x, y, z));
	}

	const std::vector<T>& get_cell_values(int64_t cx, int64_t cy, int64_t cz) const {
		return get_cell_values(_hash(cx, cy, cz));
	}

	const std::vector<T>& get_cell_values(uint64_t cell_id) const {
		const auto it = _map.find(cell_id);
		if (it != _map.end()) {
			return it->second;
		}
		const static std::vector<T> e;
		return e;
	}

	template<typename CallbackT>
	void for_each_cell(const CallbackT& cell_callback) const {
		for (const auto& e : _map) {
			cell_callback(e.second);
		}
	}

	template <typename CallbackT>
	void for_each_value(const CallbackT& value_callback) const {
		for_each_cell([&value_callback](const std::vector<T>& cell) {
			for (const auto& value : cell) {
				value_callback(value);
			}
		});
	}

protected:
	double _inv_cell_size;

private:
	void _add_to_cell(const T& shv, uint64_t cell_id) {
		const auto it = _map.find(cell_id);
		if (it == _map.end()) {
			_map[cell_id] = {shv};
		}
		else {
			it->second.push_back(shv);
		}
		++_value_count;
	}

	void _remove_from_cell(const T& shv, int64_t cell_id) {
		const auto it = _map.find(cell_id);
		assertion(it != _map.end(), "Could not find point value std::vector");
		std::vector<T>& values = it->second;

		const size_t size_before = values.size();
		auto value_it = std::find(values.begin(), values.end(), shv);
		assertion(value_it != values.end(), "Could not find value in cell");
		values.erase(value_it);
		assertion(values.size() == size_before - 1, "Could not delete a point hash value");

		if (values.empty()) {
			_map.erase(it);
		}
		--_value_count;
		assertion(_value_count >= 0, "Removed too many point values");
	}

	uint64_t _hash(const vec3& pos) const {
		return _hash(pos.x, pos.y, pos.z);
	}

	uint64_t _hash(double x, double y, double z) const {
		const auto cx = floor_to<uint64_t>(x * _inv_cell_size);
		const auto cy = floor_to<uint64_t>(y * _inv_cell_size);
		const auto cz = floor_to<uint64_t>(z * _inv_cell_size);
		return hash_good(cx, cy, cz);
	}

	static uint64_t _hash(int64_t cx, int64_t cy, int64_t cz) {
		return hash_good(cx, cy, cz);
	}

	std::unordered_map<uint64_t, std::vector<T>> _map;
	double _cell_size;
	int64_t _value_count = 0;
};
}
