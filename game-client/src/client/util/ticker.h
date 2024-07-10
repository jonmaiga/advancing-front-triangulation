#pragma once

#include "core/util/timer.h"

namespace playchilla {
struct tick_data {
	uint64_t tick{};
	double alpha{};
};

using tick_callback = std::function<void(const tick_data&)>;

class ticker {
	timer _timer;
	tick_callback _tick_callback;
	uint64_t _tick_interval_ms;
	uint64_t _last_time;
	uint64_t _acc = 0;
	uint64_t _tick = 0;
	double _alpha = 0;

public:
	ticker(double ticks_per_second, tick_callback tick_callback) :
		_tick_callback(std::move(tick_callback)),
		_tick_interval_ms(static_cast<uint64_t>(1000 / ticks_per_second)),
		_last_time(_timer.millie_seconds() - _tick_interval_ms) {
	}

	uint64_t get_tick() const {
		return _tick;
	}

	double get_alpha() const {
		return _alpha;
	}

	void step() {
		const uint64_t now = _timer.millie_seconds();
		_acc += now - _last_time;
		_acc = std::min(_acc, std::max(static_cast<uint64_t>(100), _tick_interval_ms));

		while (_acc >= _tick_interval_ms) {
			++_tick;
			_acc -= _tick_interval_ms;
			_tick_callback({_tick, 0});
		}
		_alpha = static_cast<double>(_acc) / static_cast<double>(_tick_interval_ms);
		_last_time = now;
	}
};
}
