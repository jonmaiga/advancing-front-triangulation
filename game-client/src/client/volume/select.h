#pragma once

#include <functional>

namespace playchilla::volumes {
class select : public volume {
public:
	using condition = std::function<double(double)>;

	select(
		const volume* first,
		const volume* second,
		const volume* controller,
		std::function<double(double)> condition) :
		_first(first),
		_second(second),
		_controller(controller),
		_condition(std::move(condition)) {
	}

	double get_value(double x, double y, double z) const override {
		const double control_value = _controller->get_value(x, y, z);
		const double alpha = _condition(control_value);
		if (alpha <= 0.) {
			return _first->get_value(x, y, z);
		}
		if (alpha >= 1.) {
			return _second->get_value(x, y, z);
		}
		return (1. - alpha) * _first->get_value(x, y, z) + alpha * _second->get_value(x, y, z);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		const double control_value = _controller->get_value(pos.x, pos.y, pos.z);
		const double alpha = _condition(control_value);
		if (alpha > 0) {
			return _second->get_data(pos, data);
		}
		_first->get_data(pos, data);
	}

private:
	const volume* _first;
	const volume* _second;
	const volume* _controller;
	condition _condition;
};
}
