#pragma once

#include <iostream>

namespace playchilla {

class log {
public:
	log() = default;

	log(bool enable) : _enabled(enable) {
	}

	log& enable() {
		_enabled = true;
		return *this;
	}

	log& disable() {
		_enabled = false;
		return *this;
	}

	template <typename T>
	log& operator <<(const T& t) {
		if (_enabled) {
			std::cout << t;
			std::cout << std::flush;
		}
		return *this;
	}

private:
	bool _enabled = true;
};

log& logger();
log& dummy_log();

}
