#pragma once

#include "quat.h"
#include "vec3.h"

namespace playchilla {
/**
 * Most code from libGdx quaternion.
 * http://lolengine.net/blog/2014/02/24/quaternion-from-two-vectors-final
 */

// from http://lolengine.net/blog/2014/02/24/quaternion-from-two-vectors-final
inline quat set_from_two_vectors(const vec3& u, const vec3& v) {
	const double uv_norm = std::sqrt(u.length_sqr() * v.length_sqr());
	double real = uv_norm + u.dot(v);

	vec3 tmp;
	if (real < Epsilon * uv_norm) {
		real = 0.;
		if (std::abs(u.x) > std::abs(u.z)) {
			tmp = {-u.y, u.x, 0.};
		}
		else {
			tmp = {0., -u.z, u.y};
		}
	}
	else {
		tmp = u.cross(v);
	}
	quat out;
	return out.set(tmp.x, tmp.y, tmp.z, real).normalize();

	// Normalized:
	//        double m = std::sqrt(2. + 2. * u.dot(v));
	//        _vecTmp.set(u).crossSelf(v).scaleSelf(1. / m);
	//        return out.set(_vecTmp.x, _vecTmp.y, _vecTmp.z, 0.5 * m);
}

inline quat set_from_axis_rad(double x, double y, double z, double radians) {
	double d = std::sqrt(x * x + y * y + z * z);
	if (d == 0.) {
		return {};
	}
	d = 1. / d;
	double l_ang = radians;
	double l_sin = std::sin(l_ang / 2);
	double l_cos = std::cos(l_ang / 2);
	quat out;
	return out.set(d * x * l_sin, d * y * l_sin, d * z * l_sin, l_cos).normalize();
}

inline quat set_euler_angles_rad(double pitch, double yaw, double roll) {
	double hr = roll * 0.5;
	double shr = std::sin(hr);
	double chr = std::cos(hr);
	double hp = pitch * 0.5;
	double shp = std::sin(hp);
	double chp = std::cos(hp);
	double hy = yaw * 0.5;
	double shy = std::sin(hy);
	double chy = std::cos(hy);
	double chy_shp = chy * shp;
	double shy_chp = shy * chp;
	double chy_chp = chy * chp;
	double shy_shp = shy * shp;

	double x = (chy_shp * chr) + (shy_chp * shr); // cos(yaw/2) * sin(pitch/2) * cos(roll/2) + sin(yaw/2) * cos(pitch/2) * sin(roll/2)
	double y = (shy_chp * chr) - (chy_shp * shr); // sin(yaw/2) * cos(pitch/2) * cos(roll/2) - cos(yaw/2) * sin(pitch/2) * sin(roll/2)
	double z = (chy_chp * shr) - (shy_shp * chr); // cos(yaw/2) * cos(pitch/2) * sin(roll/2) - sin(yaw/2) * sin(pitch/2) * cos(roll/2)
	double w = (chy_chp * chr) + (shy_shp * shr); // cos(yaw/2) * cos(pitch/2) * cos(roll/2) + sin(yaw/2) * sin(pitch/2) * sin(roll/2)
	return {x, y, z, w};
}

inline quat set_from_axis_deg(const vec3& axis, double degrees) {
	return set_from_axis_rad(axis.x, axis.y, axis.z, deg_to_rad(degrees));
}

inline quat set_euler_angles(double pitch, double yaw, double roll) {
	return set_euler_angles_rad(deg_to_rad(pitch), deg_to_rad(yaw), deg_to_rad(roll));
}

/**
 * Rotate
 */
inline vec3 rotate_vec3(const vec3& v, const quat& q) {
	double x1 = q.y * v.z - q.z * v.y;
	double y1 = q.z * v.x - q.x * v.z;
	double z1 = q.x * v.y - q.y * v.x;
	double x2 = q.w * x1 + q.y * z1 - q.z * y1;
	double y2 = q.w * y1 + q.z * x1 - q.x * z1;
	double z2 = q.w * z1 + q.x * y1 - q.y * x1;
	double xx = v.x + 2.0f * x2;
	double yy = v.y + 2.0f * y2;
	double zz = v.z + 2.0f * z2;
	return {xx, yy, zz};
}
}
