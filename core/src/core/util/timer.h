#pragma once

#include <chrono>
#include <functional>
#include <utility>
#include <string>

namespace playchilla {
class timer {
public:
	timer() { reset(); }
	~timer() = default;
	timer(const timer&) = delete;
	timer(timer&&) = delete;
	timer& operator=(const timer&) = delete;
	timer& operator=(timer&&) = delete;

	long long millie_seconds() const {
		return _passed<std::chrono::milliseconds>(std::chrono::steady_clock::now());
	}

	long long nano_seconds() const {
		return _passed<std::chrono::nanoseconds>(std::chrono::steady_clock::now());
	}

	long long seconds() const {
		return _passed<std::chrono::seconds>(std::chrono::steady_clock::now());
	}

	void reset() {
		_start = std::chrono::steady_clock::now();
	}

private:
	template <typename T>
	long long _passed(std::chrono::steady_clock::time_point stop) const {
		using namespace std::chrono;
		return duration_cast<T>(stop - _start).count();
	}

	std::chrono::steady_clock::time_point _start;
};


class scope_timer {
public:
	using callback = std::function<void(const timer&)>;

	scope_timer(callback callback) : _callback(std::move(callback)) {
	}

	scope_timer(const timer&) = delete;
	scope_timer(timer&&) = delete;
	scope_timer& operator=(const timer&) = delete;
	scope_timer& operator=(timer&&) = delete;

	~scope_timer() {
		_callback(_timer);
	}

private:
	timer _timer;
	callback _callback;
};

inline std::string nano_to_unit(const long long ns) {
	if (ns < 1000) {
		return std::to_string(ns) + "ns";
	}
	const auto micro = ns / 1000;
	if (micro < 1000) {
		return std::to_string(micro) + "μs";
	}
	const auto ms = micro / 1000;
	if (ms < 1000) {
		return std::to_string(ms) + "ms";
	}
	const auto sec = ms / 1000;
	if (sec < 60) {
		return std::to_string(sec) + "s";
	}
	const auto min = sec / 60;
	if (min < 60) {
		return std::to_string(min) + "m";
	}
	const auto h = min / 60;
	if (h < 24) {
		return std::to_string(h) + "h " + std::to_string(min - h * 60) + "m";
	}
	const auto d = h / 24;
	return std::to_string(d) + "d " + std::to_string(h - d * 24) + "h";
}
}
