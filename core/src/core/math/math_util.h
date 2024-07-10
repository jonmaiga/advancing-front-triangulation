#pragma once

#include <cmath>

namespace playchilla {
static constexpr double Pi = 3.14159265358979323846264338327950288419716939;
static constexpr double Epsilon = 0.0000001;
static constexpr double EpsilonSqr = Epsilon * Epsilon;

inline double min(double a, double b) {
	return a <= b ? a : b;
}

inline double min(double a, double b, double c) {
	return min(a, min(b, c));
}

inline double max(double a, double b) {
	return a <= b ? b : a;
}

inline double max(double a, double b, double c) {
	return max(a, max(b, c));
}

inline double deg_to_rad(double degrees) {
	static constexpr double d2r = Pi / 180;
	return degrees * d2r;
}

inline double rad_to_deg(double radians) {
	static constexpr double r2d = 180 / Pi;
	return radians * r2d;
}

inline bool is_near(double a, double b, double eps) {
	return std::abs(a - b) < eps;
}

inline bool is_near(double a, double b) {
	return is_near(a, b, Epsilon);
}

template <typename T>
T floor_to(double v) {
	return static_cast<T>(std::floor(v));
}

template <typename T>
T ceil_to(double v) {
	return static_cast<T>(std::ceil(v));
}

inline bool is_valid(double v) {
	return !(std::isnan(v) || std::isinf(v));
}
}
