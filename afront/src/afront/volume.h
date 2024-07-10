#pragma once

#include <memory>
#include <vector>

#include "core/math/vec3.h"

namespace playchilla {
class volume_data;

class volume {
public:
	// todo: Minimum normalizable double number
	static constexpr double MIN_VALUE = -1e100;
	static constexpr double MAX_VALUE = 1e100;

	volume() = default;
	volume(const volume&) = delete;
	volume(volume&&) = delete;
	volume& operator=(const volume&) = delete;
	volume& operator=(volume&&) = delete;
	virtual ~volume() = default;

	double get_value(const vec3& pos) const {
		return get_value(pos.x, pos.y, pos.z);
	}

	virtual double get_value(double x, double y, double z) const = 0;
	virtual void get_data(const vec3& pos, volume_data&) const = 0;
};

using volume_unique_ptr = std::unique_ptr<volume>;
using volume_unique_ptrs = std::vector<std::unique_ptr<volume>>;
}
