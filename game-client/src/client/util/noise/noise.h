#pragma once

#include "tables.h"
#include "core/math/math_util.h"

namespace playchilla {
class gradient_noise {
public:
	gradient_noise(uint64_t seed) : _seed_add(1013 * seed) {
	}

	double get_value(const double x, const double y, const double z) const {
		auto cx0 = floor_to<int64_t>(x);
		auto cy0 = floor_to<int64_t>(y);
		auto cz0 = floor_to<int64_t>(z);
		const double dx0 = x - cx0;
		const double dy0 = y - cy0;
		const double dz0 = z - cz0;
		const double xs = _quintic(dx0);
		const double ys = _quintic(dy0);
		const double zs = _quintic(dz0);

		const double dx1 = dx0 - 1.;
		const double dy1 = dy0 - 1.;
		const double dz1 = dz0 - 1.;
		cx0 *= 1619ll;
		cy0 *= 31337ll;
		cz0 = cz0 * 6971ll + _seed_add;
		const int64_t cx1 = cx0 + 1619ll;
		const int64_t cy1 = cy0 + 31337ll;
		const int64_t cz1 = cz0 + 6971ll;

		const double n00 = _lerp(_n(cx0, cy0, cz0, dx0, dy0, dz0), _n(cx1, cy0, cz0, dx1, dy0, dz0), xs);
		const double n10 = _lerp(_n(cx0, cy1, cz0, dx0, dy1, dz0), _n(cx1, cy1, cz0, dx1, dy1, dz0), xs);
		const double n01 = _lerp(_n(cx0, cy0, cz1, dx0, dy0, dz1), _n(cx1, cy0, cz1, dx1, dy0, dz1), xs);
		const double n11 = _lerp(_n(cx0, cy1, cz1, dx0, dy1, dz1), _n(cx1, cy1, cz1, dx1, dy1, dz1), xs);
		const double yf0 = _lerp(n00, n10, ys);
		const double yf1 = _lerp(n01, n11, ys);
		return 1.3 * _lerp(yf0, yf1, zs);
	}

private:
	static double _n(const int64_t cx, const int64_t cy, const int64_t cz, const double dx, const double dy, const double dz) {
		int64_t i = cx ^ cy ^ cz;
		i ^= (i >> 8);
		i &= 0xff;
		i *= 4;
		return dx * tables::random_vectors_3d[i] + dy * tables::random_vectors_3d[i + 1] + dz * tables::random_vectors_3d[i + 2];
	}

	static double _lerp(double from, double to, double a) {
		return from + a * (to - from);
	}

	static double _quintic(double t) {
		return t * t * t * (t * (t * 6. - 15.) + 10.);
	}

	uint64_t _seed_add;
};
}
