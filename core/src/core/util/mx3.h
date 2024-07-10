#pragma once

#include "conversion.h"
#include "core/math/math_util.h"

namespace playchilla::mx3 {
static constexpr uint64_t C = 0xbea225f9eb34556d;

inline uint64_t mix(uint64_t x) {
	x ^= x >> 32;
	x *= C;
	x ^= x >> 29;
	x *= C;
	x ^= x >> 32;
	x *= C;
	x ^= x >> 29;
	return x;
}

class random {
public:
	explicit random(uint64_t seed) : _counter(seed) {
	}

	uint64_t operator()() { return mix(_counter++); }

	double next_unit() {
		const uint64_t u64 = 0x3FF0000000000000 | ((mix(_counter++) >> 12) | 1);
		return util::bits_to<double>(u64) - 1.0;
	}

	double between(double from, double to) {
		return from + next_unit() * (to - from);
	}

	double normal(double mean = 1., double std_dev = 0.) {
		double u = next_unit();
		while (is_near(u, 0)) {
			u = next_unit();
		}
		double v = next_unit();
		while (is_near(v, 0)) {
			v = next_unit();
		}
		const double x = std::sqrt(-2.0 * std::log(u)) * std::cos(2.0 * Pi * v);
		return (x * std_dev) + mean;
	}

private:
	uint64_t _counter;
};
}
