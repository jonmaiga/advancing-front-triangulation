#pragma once

#include <cmath>
#include <ostream>

#include "math_util.h"
#include "core/util/mx3.h"

namespace playchilla {
class vec3 {
public:
	double x = 0, y = 0, z = 0;

	vec3() = default;

	vec3(double x, double y, double z) : x(x), y(y), z(z) {
	}

	/**
	 * Add
	 */
	vec3& operator +=(const vec3& pos) {
		return add_self(pos.x, pos.y, pos.z);
	}

	vec3& add_self(double x, double y, double z) {
		this->x += x;
		this->y += y;
		this->z += z;
		return *this;
	}

	/**
	 * Sub
	 */
	vec3& operator -=(const vec3& pos) {
		return sub_self(pos.x, pos.y, pos.z);
	}

	vec3& sub_self(double x, double y, double z) {
		this->x -= x;
		this->y -= y;
		this->z -= z;
		return *this;
	}

	/**
	 * Mul
	 */
	vec3& operator *=(double s) {
		return mul_self(s, s, s);
	}

	vec3& operator *=(const vec3& vec) {
		return mul_self(vec.x, vec.y, vec.z);
	}

	vec3& mul_self(double x, double y, double z) {
		this->x *= x;
		this->y *= y;
		this->z *= z;
		return *this;
	}

	/**
	 * Div
	 */
	vec3& operator /=(double s) {
		return div_self(s, s, s);
	}

	vec3& operator /=(const vec3& vec) {
		return div_self(vec.x, vec.y, vec.z);
	}

	vec3& div_self(double x, double y, double z) {
		this->x /= x;
		this->y /= y;
		this->z /= z;
		return *this;
	}

	/**
	 * Normalize
	 */
	vec3& normalize_self() & {
		const double nf = 1. / std::sqrt(x * x + y * y + z * z);
		x *= nf;
		y *= nf;
		z *= nf;
		return *this;
	}

	vec3& normalize_self(const vec3& fallback) & {
		const double dist_sqr = x * x + y * y + z * z;
		if (dist_sqr < EpsilonSqr) {
			x = fallback.x;
			y = fallback.y;
			z = fallback.z;
		}
		else {
			const double nf = 1. / std::sqrt(dist_sqr);
			x *= nf;
			y *= nf;
			z *= nf;
		}
		return *this;
	}

	[[nodiscard]] vec3 operator-() const {
		return {-x, -y, -z};
	}

	vec3 xzy() {
		return {x, z, y};
	}

	vec3 yxz() {
		return {y, z, x};
	}

	vec3 zyx() {
		return {z, y, x};
	}

	vec3 zxy() {
		return {z, x, y};
	}

	/**
	 * Const
	 */

	[[nodiscard]] vec3 operator +(const vec3& pos) const {
		return {x + pos.x, y + pos.y, z + pos.z};
	}

	[[nodiscard]] vec3 add(double x, double y, double z) const {
		return {this->x + x, this->y + y, this->z + z};
	}

	[[nodiscard]] vec3 operator -(const vec3& pos) const {
		return {x - pos.x, y - pos.y, z - pos.z};
	}

	[[nodiscard]] vec3 sub(double x, double y, double z) const {
		return {this->x - x, this->y - y, this->z - z};
	}

	[[nodiscard]] vec3 operator *(const vec3& vec) const {
		return {x * vec.x, y * vec.y, z * vec.z};
	}

	[[nodiscard]] vec3 mul(double x, double y, double z) const {
		return {this->x * x, this->y * y, this->z * z};
	}

	[[nodiscard]] vec3 operator /(const vec3& vec) const {
		return {x / vec.x, y / vec.y, z / vec.z};
	}

	[[nodiscard]] vec3 div(double x, double y, double z) const {
		return {this->x / x, this->y / y, this->z / z};
	}

	/**
	 * Scale
	 */
	[[nodiscard]] vec3 operator *(double s) const {
		return scale(s);
	}

	[[nodiscard]] vec3 scale(double s) const {
		return {x * s, y * s, z * s};
	}

	[[nodiscard]] vec3 rescale(double new_length) const {
		const double nf = new_length / std::sqrt(x * x + y * y + z * z);
		return {x * nf, y * nf, z * nf};
	}

	/**
	 * Normalize
	 */
	[[nodiscard]] vec3 normalize() const {
		const double nf = 1 / std::sqrt(x * x + y * y + z * z);
		return {x * nf, y * nf, z * nf};
	}

	[[nodiscard]] vec3 normalize(const vec3& fallback) const {
		const double d2 = x * x + y * y + z * z;
		if (d2 < EpsilonSqr) {
			return fallback;
		}
		const double nf = 1 / std::sqrt(d2);
		return {x * nf, y * nf, z * nf};
	}


	/**
	 * Distance
	 */
	[[nodiscard]] double length() const {
		return std::sqrt(length_sqr());
	}

	[[nodiscard]] double length_sqr() const {
		return x * x + y * y + z * z;
	}

	[[nodiscard]] double distance(const vec3& vec) const {
		return distance(vec.x, vec.y, vec.z);
	}

	[[nodiscard]] double distance(double x, double y, double z) const {
		const double xd = this->x - x;
		const double yd = this->y - y;
		const double zd = this->z - z;
		return std::sqrt(xd * xd + yd * yd + zd * zd);
	}

	[[nodiscard]] double distance_sqr(const vec3& vec) const {
		return distance_sqr(vec.x, vec.y, vec.z);
	}

	[[nodiscard]] double distance_sqr(double x, double y, double z) const {
		const double xd = this->x - x;
		const double yd = this->y - y;
		const double zd = this->z - z;
		return xd * xd + yd * yd + zd * zd;
	}

	[[nodiscard]] bool is_exactly(const vec3& v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	[[nodiscard]] bool operator ==(const vec3& vec) const {
		return equals(vec.x, vec.y, vec.z);
	}

	[[nodiscard]] bool operator !=(const vec3& vec) const {
		return !equals(vec.x, vec.y, vec.z);
	}

	[[nodiscard]] bool equals(double x, double y, double z) const {
		return this->x == x && this->y == y && this->z == z;
	}

	[[nodiscard]] bool is_normalized() const {
		return std::abs(x * x + y * y + z * z - 1) < EpsilonSqr;
	}

	[[nodiscard]] bool is_exactly_zero() const {
		return x == 0 && y == 0 && z == 0;
	}

	[[nodiscard]] bool is_near(const vec3& vec) const {
		return distance_sqr(vec) < EpsilonSqr;
	}

	[[nodiscard]] bool is_near(double x, double y, double z) const {
		return distance_sqr(x, y, z) < EpsilonSqr;
	}

	[[nodiscard]] bool is_within(const vec3& vec, double epsilon) const {
		return is_within(vec.x, vec.y, vec.z, epsilon);
	}

	[[nodiscard]] bool is_within(double x, double y, double z, double epsilon) const {
		return distance_sqr(x, y, z) < epsilon * epsilon;
	}

	[[nodiscard]] bool is_valid() const {
		return !is_nan() && !is_inf();
	}

	[[nodiscard]] bool is_nan() const {
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	[[nodiscard]] bool is_inf() const {
		return std::isinf(x) || std::isinf(y) || std::isinf(z);
	}

	/**
	 * Dot product
	 */
	[[nodiscard]] double dot(const vec3& vec) const {
		return dot(vec.x, vec.y, vec.z);
	}

	[[nodiscard]] double dot(double x, double y, double z) const {
		return this->x * x + this->y * y + this->z * z;
	}

	/**
	 * Cross determinant
	 */
	[[nodiscard]] vec3 cross(const vec3& v) const {
		return cross(v.x, v.y, v.z);
	}

	[[nodiscard]] vec3 cross(double x, double y, double z) const {
		return {this->y * z - y * this->z, this->z * x - z * this->x, this->x * y - x * this->y};
	}

	[[nodiscard]] vec3 negate() const {
		return {-x, -y, -z};
	}

	[[nodiscard]] vec3 inverse() const {
		return {1. / x, 1. / y, 1. / z};
	}

	/**
	 * Lerp / slerp
	 * Note: Slerp instanceof not well tested yet.
	 */
	[[nodiscard]] vec3 lerp(const vec3& to, double t) const {
		return {x + t * (to.x - x), y + t * (to.y - y), z + t * (to.z - z)};
	}

	[[nodiscard]] vec3 slerp(const vec3& vec, double t) const {
		const double cos_theta = dot(vec);
		const double theta = std::acos(cos_theta);
		const double sin_theta = std::sin(theta);
		if (sin_theta <= Epsilon) {
			return vec;
		}
		const double w1 = std::sin((1 - t) * theta) / sin_theta;
		const double w2 = std::sin(t * theta) / sin_theta;
		return scale(w1) + vec.scale(w2);
	}

	[[nodiscard]] vec3 reflect(const vec3& normal) const {
		const double d = 2 * (x * normal.x + y * normal.y + z * normal.z);
		return {x - d * normal.x, y - d * normal.y, z - d * normal.z};
	}

	[[nodiscard]] vec3 get_min(const vec3& p) const {
		return {min(p.x, x), min(p.y, y), min(p.z, z)};
	}

	[[nodiscard]] vec3 get_max(const vec3& p) const {
		return {max(p.x, x), max(p.y, y), max(p.z, z)};
	}

	[[nodiscard]] vec3 floor() const {
		return {std::floor(x), std::floor(y), std::floor(z)};
	}

	[[nodiscard]] double max_component_length() const {
		return max(std::abs(x), max(std::abs(y), std::abs(z)));
	}

	[[nodiscard]] double min_component() const {
		return min(x, min(y, z));
	}

	[[nodiscard]] double max_component() const {
		return max(x, max(y, z));
	}

	[[nodiscard]] vec3 orthogonal() const {
		if (std::abs(x) < Epsilon && std::abs(y) < Epsilon) {
			return {0, 0, 1};
		}
		return {-y, x, z};
	}
};

inline vec3 operator* (double s, const vec3& v) {
	return v * s;
}

namespace vec3d {
inline const vec3 zero(0, 0, 0);
inline const vec3 X(1, 0, 0);
inline const vec3 Y(0, 1, 0);
inline const vec3 Z(0, 0, 1);

inline vec3 create_random_dir(double a, double b) {
	const double rads = a * Pi * 2;
	const double z = 2 * b - 1;
	const double zp = std::sqrt(1 - z * z);
	return {zp * std::cos(rads), zp * std::sin(rads), z};
}

inline vec3 create_random_dir(mx3::random& r) {
	return create_random_dir(r.next_unit(), r.next_unit());
}

inline vec3 min(const vec3& a, const vec3& b) {
	return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}

inline vec3 max(const vec3& a, const vec3& b) {
	return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}
}

inline std::ostream& operator <<(std::ostream& os, const vec3& p) {
	return os << "[" << p.x << ", " << p.y << ", " << p.z << "]";
}
}
