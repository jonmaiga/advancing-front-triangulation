#pragma once

#include "math_util.h"

namespace playchilla {
class quat {
public:
	static const quat Identity;

	double x = 0;
	double y = 0;
	double z = 0;
	double w = 1;

	quat() = default;

	quat(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {
	}

	quat& set(double qx, double qy, double qz, double qw) {
		x = qx;
		y = qy;
		z = qz;
		w = qw;
		return *this;
	}

	double length() const {
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	double length_sqr() const {
		return x * x + y * y + z * z + w * w;
	}

	quat normalize() const {
		const double f = 1. / length();
		return {x * f, y * f, z * f, w * f};
	}

	bool is_normalized() const {
		return std::abs((x * x + y * y + z * z + w * w) - 1) < EpsilonSqr;
	}

	quat add(const quat& q) const {
		return {x + q.x, y + q.y, z + q.z, w + q.w};
	}

	quat add(double qx, double qy, double qz, double qw) const {
		return {x + qx, y + qy, z + qz, w + qw};
	}

	quat scale(double s) const {
		return {x * s, y * s, z * s, w * s};
	}

	double dot(const quat& other) const {
		return dot(other.x, other.y, other.z, other.w);
	}

	double dot(double qx, double qy, double qz, double qw) const {
		return x * qx + y * qy + z * qz + w * qw;
	}

	quat mul(const quat& other) const {
		return mul(other.x, other.y, other.z, other.w);
	}

	quat mul(double qx, double qy, double qz, double qw) const {
		const double newX = w * qx + x * qw + y * qz - z * qy;
		const double newY = w * qy + y * qw + z * qx - x * qz;
		const double newZ = w * qz + z * qw + x * qy - y * qx;
		const double newW = w * qw - x * qx - y * qy - z * qz;
		return {newX, newY, newZ, newW};
	}

	quat mul_left(const quat& other) const {
		return mul_left(other.x, other.y, other.z, other.w);
	}

	quat mul_left(double qx, double qy, double qz, double qw) const {
		const double newX = qw * x + qx * w + qy * z - qz * y;
		const double newY = qw * y + qy * w + qz * x - qx * z;
		const double newZ = qw * z + qz * w + qx * y - qy * x;
		const double newW = qw * w - qx * x - qy * y - qz * z;
		return {newX, newY, newZ, newW};
	}

	quat conjugate() const {
		return {-x, -y, -z, w};
	}

	bool is_exactly(const quat& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	double get_degrees() const {
		return rad_to_deg(get_radians());
	}

	double get_radians() const {
		return 2.0 * std::acos((w > 1) ? (w / length()) : w);
	}

	double get_around_x_degrees() const {
		return rad_to_deg(get_around_x_radians());
	}

	double get_around_y_degrees() const {
		return rad_to_deg(get_around_y_radians());
	}

	double get_around_z_degrees() const {
		return rad_to_deg(get_around_z_radians());
	}

	// roll
	double get_around_x_radians() const {
		return std::atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z);
	}

	// pitch
	double get_around_y_radians() const {
		return std::asin(-2 * (x * z - w * y));
	}

	// yaw
	double get_around_z_radians() const {
		return std::atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z);
	}

	quat lerp(const quat& end_quat, double alpha) const {
		quat delta(*this);
		delta = delta.scale(1 - alpha);
		delta = delta.add(end_quat.scale(alpha));
		return delta.normalize();
	}

	quat slerp(const quat& end, double alpha) const {
		const double d = x * end.x + y * end.y + z * end.z + w * end.w;
		const double abs_dot = d < 0. ? -d : d;
		double scale0 = 1. - alpha;
		double scale1 = alpha;
		if ((1. - abs_dot) > 0.1) {
			const double angle = std::acos(abs_dot);
			const double inv_sin_theta = 1. / std::sin(angle);
			scale0 = std::sin((1. - alpha) * angle) * inv_sin_theta;
			scale1 = std::sin((alpha * angle)) * inv_sin_theta;
		}

		if (d < 0.f) {
			scale1 = -scale1;
		}

		return quat(
			(scale0 * x) + (scale1 * end.x),
			(scale0 * y) + (scale1 * end.y),
			(scale0 * z) + (scale1 * end.z),
			(scale0 * w) + (scale1 * end.w)).normalize();
	}
};
}
