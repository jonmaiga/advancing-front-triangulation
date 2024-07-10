#pragma once

#include "core/debug/assertion.h"
#include "core/math/matrix4.h"
#include "core/math/vec3.h"

namespace playchilla {
class relative_camera {
	matrix4 _view;
	matrix4 _combined;
	matrix4 _projection;
	double _width;
	double _height;
	double _near;
	double _far;
	double _fov;
	vec3 _pos;

public:
	relative_camera(double width, double height, double near, double far, double fov, const vec3& pos = {}) {
		_width = width;
		_height = height;
		_near = near;
		_far = far;
		_fov = fov;

		far = std::abs(far);
		near = std::abs(near);
		double aspect = width / height;
		double lfd = (1.0 / std::tan((fov * (Pi / 180.0)) / 2.0));
		double la1 = (far + near) / (near - far);
		double la2 = (2.0 * far * near) / (near - far);
		_projection.m00 = lfd / aspect;
		_projection.m10 = 0;
		_projection.m20 = 0;
		_projection.m30 = 0;
		_projection.m01 = 0;
		_projection.m11 = lfd;
		_projection.m21 = 0;
		_projection.m31 = 0;
		_projection.m02 = 0;
		_projection.m12 = 0;
		_projection.m22 = la1;
		_projection.m32 = -1.0;
		_projection.m03 = 0;
		_projection.m13 = 0;
		_projection.m23 = la2;
		_projection.m33 = 0;

		update(pos, {0, 1, 0}, {0, 0, 1});
	}

	double width() const {
		return _width;
	}

	double height() const {
		return _height;
	}

	double fov() const {
		return _fov;
	}

	const vec3& get_pos() const {
		return _pos;
	}

	void update(const vec3& pos, const vec3& up, const vec3& forward) {
		assertion(pos.is_valid(), "Invalid pos when updating camera");
		assertion(up.is_valid(), "Invalid up when updating camera");
		assertion(forward.is_valid(), "Invalid fwd when updating camera");
		_pos = pos;
		const vec3& right = forward.cross(up).normalize();
		assertion(right.is_valid(), "Invalid right when updating camera");
		_view.m00 = right.x;
		_view.m01 = right.y;
		_view.m02 = right.z;
		_view.m03 = 0;

		_view.m10 = up.x;
		_view.m11 = up.y;
		_view.m12 = up.z;
		_view.m13 = 0;

		_view.m20 = -forward.x;
		_view.m21 = -forward.y;
		_view.m22 = -forward.z;
		_view.m23 = 0;

		_combined = _projection.mul(_view);
	}

	const matrix4& get_combined() const {
		return _combined;
	}

	const matrix4& get_view() const {
		return _view;
	}

	const matrix4& get_projection() const {
		return _projection;
	}

	vec3 project(const vec3& p) const {
		const matrix4& combined = get_combined();
		double iw = 1. / (p.dot(combined.m30, combined.m31, combined.m32) + combined.m33);
		double x = p.dot(combined.m00, combined.m01, combined.m02) + combined.m03 * iw;
		double y = p.dot(combined.m10, combined.m11, combined.m12) + combined.m13 * iw;
		double z = p.dot(combined.m20, combined.m21, combined.m22) + combined.m23 * iw;
		return {
			_width * 0.5 * (x + 1.),
			_height * 0.5 * (y + 1.),
			0.5 * (z + 1.)
		};
	}
};
}
