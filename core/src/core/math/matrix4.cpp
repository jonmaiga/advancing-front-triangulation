#include "matrix4.h"

namespace playchilla::internal {
matrix4 translate(const matrix4& m, double x, double y, double z) {
	// todo: optimize
	matrix4 tmp;
	tmp.set_translation(x, y, z);
	return mul(m, tmp);
}

matrix4 scale(const matrix4& m, double sx, double sy, double sz) {
	// todo: optimize
	matrix4 tmp;
	tmp.set_scale(sx, sy, sz);
	return mul(m, tmp);
}

matrix4 mul(const matrix4& lhs, const matrix4& rhs) {
	const double m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
	const double m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
	const double m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
	const double m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;
	const double m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
	const double m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
	const double m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
	const double m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;
	const double m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
	const double m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
	const double m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
	const double m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;
	const double m30 = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30;
	const double m31 = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31;
	const double m32 = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32;
	const double m33 = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33;

	matrix4 result;
	result.m00 = m00;
	result.m01 = m01;
	result.m02 = m02;
	result.m03 = m03;
	result.m10 = m10;
	result.m11 = m11;
	result.m12 = m12;
	result.m13 = m13;
	result.m20 = m20;
	result.m21 = m21;
	result.m22 = m22;
	result.m23 = m23;
	result.m30 = m30;
	result.m31 = m31;
	result.m32 = m32;
	result.m33 = m33;
	return result;
}


matrix4 invert(const matrix4& m) {
	const double nm00 = m.m12 * m.m23 * m.m31 - m.m13 * m.m22 * m.m31 + m.m13 * m.m21 * m.m32 - m.m11
		* m.m23 * m.m32 - m.m12 * m.m21 * m.m33 + m.m11 * m.m22 * m.m33;
	const double nm01 = m.m03 * m.m22 * m.m31 - m.m02 * m.m23 * m.m31 - m.m03 * m.m21 * m.m32 + m.m01
		* m.m23 * m.m32 + m.m02 * m.m21 * m.m33 - m.m01 * m.m22 * m.m33;
	const double nm02 = m.m02 * m.m13 * m.m31 - m.m03 * m.m12 * m.m31 + m.m03 * m.m11 * m.m32 - m.m01
		* m.m13 * m.m32 - m.m02 * m.m11 * m.m33 + m.m01 * m.m12 * m.m33;
	const double nm03 = m.m03 * m.m12 * m.m21 - m.m02 * m.m13 * m.m21 - m.m03 * m.m11 * m.m22 + m.m01
		* m.m13 * m.m22 + m.m02 * m.m11 * m.m23 - m.m01 * m.m12 * m.m23;
	const double nm10 = m.m13 * m.m22 * m.m30 - m.m12 * m.m23 * m.m30 - m.m13 * m.m20 * m.m32 + m.m10
		* m.m23 * m.m32 + m.m12 * m.m20 * m.m33 - m.m10 * m.m22 * m.m33;
	const double nm11 = m.m02 * m.m23 * m.m30 - m.m03 * m.m22 * m.m30 + m.m03 * m.m20 * m.m32 - m.m00
		* m.m23 * m.m32 - m.m02 * m.m20 * m.m33 + m.m00 * m.m22 * m.m33;
	const double nm12 = m.m03 * m.m12 * m.m30 - m.m02 * m.m13 * m.m30 - m.m03 * m.m10 * m.m32 + m.m00
		* m.m13 * m.m32 + m.m02 * m.m10 * m.m33 - m.m00 * m.m12 * m.m33;
	const double nm13 = m.m02 * m.m13 * m.m20 - m.m03 * m.m12 * m.m20 + m.m03 * m.m10 * m.m22 - m.m00
		* m.m13 * m.m22 - m.m02 * m.m10 * m.m23 + m.m00 * m.m12 * m.m23;
	const double nm20 = m.m11 * m.m23 * m.m30 - m.m13 * m.m21 * m.m30 + m.m13 * m.m20 * m.m31 - m.m10
		* m.m23 * m.m31 - m.m11 * m.m20 * m.m33 + m.m10 * m.m21 * m.m33;
	const double nm21 = m.m03 * m.m21 * m.m30 - m.m01 * m.m23 * m.m30 - m.m03 * m.m20 * m.m31 + m.m00
		* m.m23 * m.m31 + m.m01 * m.m20 * m.m33 - m.m00 * m.m21 * m.m33;
	const double nm22 = m.m01 * m.m13 * m.m30 - m.m03 * m.m11 * m.m30 + m.m03 * m.m10 * m.m31 - m.m00
		* m.m13 * m.m31 - m.m01 * m.m10 * m.m33 + m.m00 * m.m11 * m.m33;
	const double nm23 = m.m03 * m.m11 * m.m20 - m.m01 * m.m13 * m.m20 - m.m03 * m.m10 * m.m21 + m.m00
		* m.m13 * m.m21 + m.m01 * m.m10 * m.m23 - m.m00 * m.m11 * m.m23;
	const double nm30 = m.m12 * m.m21 * m.m30 - m.m11 * m.m22 * m.m30 - m.m12 * m.m20 * m.m31 + m.m10
		* m.m22 * m.m31 + m.m11 * m.m20 * m.m32 - m.m10 * m.m21 * m.m32;
	const double nm31 = m.m01 * m.m22 * m.m30 - m.m02 * m.m21 * m.m30 + m.m02 * m.m20 * m.m31 - m.m00
		* m.m22 * m.m31 - m.m01 * m.m20 * m.m32 + m.m00 * m.m21 * m.m32;
	const double nm32 = m.m02 * m.m11 * m.m30 - m.m01 * m.m12 * m.m30 - m.m02 * m.m10 * m.m31 + m.m00
		* m.m12 * m.m31 + m.m01 * m.m10 * m.m32 - m.m00 * m.m11 * m.m32;
	const double nm33 = m.m01 * m.m12 * m.m20 - m.m02 * m.m11 * m.m20 + m.m02 * m.m10 * m.m21 - m.m00
		* m.m12 * m.m21 - m.m01 * m.m10 * m.m22 + m.m00 * m.m11 * m.m22;

	const double inv_det = 1.0 / m.get_determinant();
	matrix4 result;
	result.m00 = nm00 * inv_det;
	result.m01 = nm01 * inv_det;
	result.m02 = nm02 * inv_det;
	result.m03 = nm03 * inv_det;
	result.m10 = nm10 * inv_det;
	result.m11 = nm11 * inv_det;
	result.m12 = nm12 * inv_det;
	result.m13 = nm13 * inv_det;
	result.m20 = nm20 * inv_det;
	result.m21 = nm21 * inv_det;
	result.m22 = nm22 * inv_det;
	result.m23 = nm23 * inv_det;
	result.m30 = nm30 * inv_det;
	result.m31 = nm31 * inv_det;
	result.m32 = nm32 * inv_det;
	result.m33 = nm33 * inv_det;
	return result;
}
}
