#pragma once

#include "afront/volume.h"

namespace playchilla::volumes {
class difference : public volume {
public:
	difference(const volume* source, std::vector<const volume*> rhs) :
		_source(source),
		_differences(std::move(rhs)) {
	}

	double get_value(double x, double y, double z) const override {
		double v = _source->get_value(x, y, z);
		for (const auto& d : _differences) {
			v = std::min(v, -d->get_value(x, y, z));
		}
		return v;
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		const volume* module = _source;
		double v = _source->get_value(pos);
		for (const auto& d : _differences) {
			const double dv = -d->get_value(pos);
			if (dv < v) {
				module = d;
				v = dv;
			}
		}
		module->get_data(pos, data);
	}

private:
	const volume* _source;
	std::vector<const volume*> _differences;
};
}
