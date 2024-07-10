#pragma once

#include "core/math/math_util.h"
#include "core/util/conversion.h"

namespace playchilla {
class rgba {
public:
	rgba() = default;

	rgba(double r, double g, double b) : rgba(r, g, b, 1) {
	}

	rgba(double r, double g, double b, double a) : r(r), g(g), b(b), a(a) {
	}

	uint32_t to_rgba_8888() const {
		const uint32_t red = static_cast<uint32_t>(255 * r) << 24;
		const uint32_t green = static_cast<uint32_t>(255 * g) << 16;
		const uint32_t blue = static_cast<uint32_t>(255 * b) << 8;
		const uint32_t alpha = static_cast<uint32_t>(255 * a);
		return red + green + blue + alpha;
	}

	uint32_t to_rgb_888() const {
		const uint32_t red = static_cast<uint32_t>(255 * r) << 16;
		const uint32_t green = static_cast<uint32_t>(255 * g) << 8;
		const uint32_t blue = static_cast<uint32_t>(255 * b);
		return red + green + blue;
	}

	uint32_t to_abgr_8888() const {
		return
			(static_cast<uint32_t>(255 * a) << 24) |
			(static_cast<uint32_t>(255 * b) << 16) |
			(static_cast<uint32_t>(255 * g) << 8) |
			static_cast<uint32_t>(255 * r);
	}

	float to_abgr_8888_float() const {
		return util::bits_to<float>(to_abgr_8888());
	}

	float to_rgba_8888_float() const {
		return util::bits_to<float>(to_rgba_8888());
	}

	double max_rgb() const {
		return max(r, g, b);
	}

	double min_rgb() const {
		return min(r, g, b);
	}

	double get_hue() const {
		const double max = max_rgb();
		const double min = min_rgb();
		double hue = max - min;
		if (hue > 0.0) {
			if (max == r) {
				hue = (g - b) / hue;
				if (hue < 0.0) {
					hue += 6.0;
				}
			}
			else if (max == g) {
				hue = 2.0 + (b - r) / hue;
			}
			else /*max == blue*/ {
				hue = 4.0 + (r - g) / hue;
			}
			hue /= 6.0;
		}
		return hue;
	}

	double get_saturation() const {
		const double max = max_rgb();
		const double min = min_rgb();
		const double sum = max + min;
		double saturation = max - min;
		if (saturation > 0.0) {
			saturation /= (sum > 1.0) ? 2.0 - sum : sum;
		}
		return saturation;
	}

	double get_luminosity() const {
		const double max = max_rgb();
		const double min = min_rgb();
		const double sum = max + min;
		return sum / 2.0;
	}

	void set(uint32_t rgba8888) {
		r = ((rgba8888 & 0xff000000) >> 24) / 255.;
		g = ((rgba8888 & 0x00ff0000) >> 16) / 255.;
		b = ((rgba8888 & 0x0000ff00) >> 8) / 255.;
		a = (rgba8888 & 0x000000ff) / 255.;
	}

	static rgba from_compressed_abgr(float compressedABGR) {
		const auto value = util::bits_to<uint32_t>(compressedABGR);
		const double a = ((value & 0xff000000) >> 24) / 255.;
		const double b = ((value & 0x00ff0000) >> 16) / 255.;
		const double g = ((value & 0x0000ff00) >> 8) / 255.;
		const double r = ((value & 0x000000ff)) / 255.;
		return {r, g, b, a};
	}

	double r = 0, g = 0, b = 0, a = 1;
};
}
