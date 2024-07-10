#pragma once

#include "vec3.h"
#include "quat.h"

namespace playchilla {
class matrix4;

namespace internal {
matrix4 translate(const matrix4&, double x, double y, double z);
matrix4 scale(const matrix4&, double sx, double sy, double sz);
matrix4 mul(const matrix4& lhs, const matrix4& rhs);
matrix4 invert(const matrix4&);
}

class matrix4 {
public:
	double m00 = 1.;
	double m01 = 0.;
	double m02 = 0.;
	double m03 = 0.;
	double m10 = 0.;
	double m11 = 1.;
	double m12 = 0.;
	double m13 = 0.;
	double m20 = 0.;
	double m21 = 0.;
	double m22 = 1.;
	double m23 = 0.;
	double m30 = 0.;
	double m31 = 0.;
	double m32 = 0.;
	double m33 = 1.;
	static const matrix4 Identity;

	matrix4() = default;

	matrix4& set_translation(double x, double y, double z) {
		return set(x, y, z, 0, 0, 0, 1, 1, 1, 1);
	}

	matrix4& set_scale(double sx, double sy, double sz) {
		return set(0, 0, 0, 0, 0, 0, 1, sx, sy, sz);
	}

	matrix4& set_rotation(double rx, double ry, double rz, double rw) {
		return set(0, 0, 0, rx, ry, rz, rw, 1, 1, 1);
	}

	matrix4& set(double translationX, double translationY, double translationZ,
	             double quaternionX, double quaternionY, double quaternionZ, double quaternionW,
	             double scaleX, double scaleY, double scaleZ) {
		const double xs = quaternionX * 2., ys = quaternionY * 2., zs = quaternionZ * 2.;
		const double wx = quaternionW * xs, wy = quaternionW * ys, wz = quaternionW * zs;
		const double xx = quaternionX * xs, xy = quaternionX * ys, xz = quaternionX * zs;
		const double yy = quaternionY * ys, yz = quaternionY * zs, zz = quaternionZ * zs;

		m00 = scaleX * (1. - (yy + zz));
		m01 = scaleY * (xy - wz);
		m02 = scaleZ * (xz + wy);
		m03 = translationX;

		m10 = scaleX * (xy + wz);
		m11 = scaleY * (1. - (xx + zz));
		m12 = scaleZ * (yz - wx);
		m13 = translationY;

		m20 = scaleX * (xz - wy);
		m21 = scaleY * (yz + wx);
		m22 = scaleZ * (1. - (xx + yy));
		m23 = translationZ;

		m30 = 0.;
		m31 = 0.;
		m32 = 0.;
		m33 = 1.;
		return *this;
	}

	double get_determinant() const {
		return m30 * m21 * m12 * m03 - m20 * m31 * m12 * m03 - m30 * m11
			* m22 * m03 + m10 * m31 * m22 * m03 + m20 * m11 * m32 * m03 - m10
			* m21 * m32 * m03 - m30 * m21 * m02 * m13 + m20 * m31 * m02 * m13
			+ m30 * m01 * m22 * m13 - m00 * m31 * m22 * m13 - m20 * m01 * m32
			* m13 + m00 * m21 * m32 * m13 + m30 * m11 * m02 * m23 - m10 * m31
			* m02 * m23 - m30 * m01 * m12 * m23 + m00 * m31 * m12 * m23 + m10
			* m01 * m32 * m23 - m00 * m11 * m32 * m23 - m20 * m11 * m02 * m33
			+ m10 * m21 * m02 * m33 + m20 * m01 * m12 * m33 - m00 * m21 * m12
			* m33 - m10 * m01 * m22 * m33 + m00 * m11 * m22 * m33;
	}

	void decompose(vec3& translation_out, vec3& scaling_out, quat& rotation_out) const {
		translation_out = {m03, m13, m23};
		scaling_out = get_scale();
		rotation_out = get_rotation(scaling_out);
	}

	vec3 get_translation() const {
		return {m03, m13, m23};
	}

	matrix4 translate(double x, double y, double z) const {
		return internal::translate(*this, x, y, z);
	}

	vec3 get_scale() const {
		const double sx = m00 * m00 + m01 * m01 + m02 * m02;
		const double sy = m10 * m10 + m11 * m11 + m12 * m12;
		const double sz = m20 * m20 + m21 * m21 + m22 * m22;
		return {std::sqrt(sx), std::sqrt(sy), std::sqrt(sz)};
	}

	matrix4 scale(double sx, double sy, double sz) const {
		return internal::scale(*this, sx, sy, sz);
	}

	quat get_rotation() const {
		return get_rotation(get_scale());
	}

	quat get_rotation(const vec3& s) const {
		const double s00 = m00 / s.x;
		const double s01 = m01 / s.y;
		const double s02 = m02 / s.z;
		const double s10 = m10 / s.x;
		const double s11 = m11 / s.y;
		const double s12 = m12 / s.z;
		const double s20 = m20 / s.x;
		const double s21 = m21 / s.y;
		const double s22 = m22 / s.z;

		const double tr = s00 + s11 + s22;
		quat q;
		if (tr >= 0) {
			const double S = std::sqrt(tr + 1.0) * 2; // S=4*qw
			q.w = 0.25 * S;
			q.x = (s21 - s12) / S;
			q.y = (s02 - s20) / S;
			q.z = (s10 - s01) / S;
		}
		else if (s00 > s11 && s00 > s22) {
			const double S = std::sqrt(1.0 + s00 - s11 - s22) * 2; // S=4*qx
			q.w = (s21 - s12) / S;
			q.x = 0.25 * S;
			q.y = (s01 + s10) / S;
			q.z = (s02 + s20) / S;
		}
		else if (s11 > s22) {
			const double S = std::sqrt(1.0 + s11 - s00 - s22) * 2; // S=4*qy
			q.w = (s02 - s20) / S;
			q.x = (s01 + s10) / S;
			q.y = 0.25 * S;
			q.z = (s12 + s21) / S;
		}
		else {
			const double S = std::sqrt(1.0 + s22 - s00 - s11) * 2; // S=4*qz
			q.w = (s10 - s01) / S;
			q.x = (s02 + s20) / S;
			q.y = (s12 + s21) / S;
			q.z = 0.25 * S;
		}
		return q;
	}

	vec3 mul(const vec3& v) const {
		return mul(v.x, v.y, v.z);
	}

	vec3 mul(double x, double y, double z) const {
		return {
			x * m00 + y * m01 + z * m02 + m03,
			x * m10 + y * m11 + z * m12 + m13,
			x * m20 + y * m21 + z * m22 + m23
		};
	}

	matrix4 mul(const matrix4& rhs) const {
		return internal::mul(*this, rhs);
	}

	matrix4 mul_left(const matrix4& rhs) const {
		return internal::mul(rhs, *this);
	}

	matrix4 invert() const {
		return internal::invert(*this);
	}

	bool is_valid() const {
		return !is_nan() && !is_inf();
	}

	bool is_nan() const {
		return std::isnan(m00) || std::isnan(m01) || std::isnan(m02) || std::isnan(m03) ||
			std::isnan(m10) || std::isnan(m11) || std::isnan(m12) || std::isnan(m13) ||
			std::isnan(m20) || std::isnan(m21) || std::isnan(m22) || std::isnan(m23) ||
			std::isnan(m30) || std::isnan(m31) || std::isnan(m32) || std::isnan(m33);
	}

	bool is_inf() const {
		return std::isinf(m00) || std::isinf(m01) || std::isinf(m02) || std::isinf(m03) ||
			std::isinf(m10) || std::isinf(m11) || std::isinf(m12) || std::isinf(m13) ||
			std::isinf(m20) || std::isinf(m21) || std::isinf(m22) || std::isinf(m23) ||
			std::isinf(m30) || std::isinf(m31) || std::isinf(m32) || std::isinf(m33);
	}
};
}
