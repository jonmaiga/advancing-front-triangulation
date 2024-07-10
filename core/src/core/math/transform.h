#pragma once

#include "core/debug/assertion.h"
#include "core/math/matrix4.h"
#include "core/math/quat_util.h"
#include "core/math/vec3.h"

namespace playchilla::axis {
inline const vec3 Right = vec3d::X.negate();
inline const vec3 Left = Right.negate();
inline const vec3 Up = vec3d::Y;
inline const vec3 Down = Up.negate();
inline const vec3 Forward = vec3d::Z;
inline const vec3 Backward = Forward.negate();
}


namespace playchilla {
inline vec3 get_adjusted_forward(const vec3& up, const vec3& forward) {
	vec3 new_right = forward.cross(up);
	if (new_right.length_sqr() < EpsilonSqr) {
		return vec3d::zero;
	}
	new_right.normalize_self();
	return up.cross(new_right);
}


inline quat get_adjusted_rotation(const vec3& up, const vec3& forward) {
	assertion(up.is_normalized(), "Non normalized forward in get_adjusted_rotation");
	assertion(forward.is_normalized(), "Non normalized forward in get_adjusted_rotation");
	vec3 adjusted_forward = get_adjusted_forward(up, forward);
	if (adjusted_forward.length_sqr() < EpsilonSqr) {
		return quat::Identity;
	}
	adjusted_forward.normalize_self();
	quat upq = set_from_two_vectors(axis::Up, up);
	quat between = set_from_two_vectors(rotate_vec3(axis::Forward, upq).normalize(), adjusted_forward);
	between = between.mul(upq).normalize();
	return between;
}

class transform {
public:
	transform() = default;

	transform(const matrix4& local) : _local(local) {
	}

	///////////////////////
	// Translation
	///////////////////////
	double x() const {
		return _pos.x;
	}

	double y() const {
		return _pos.y;
	}

	double z() const {
		return _pos.z;
	}

	const vec3& get_pos() const {
		return _pos;
	}

	transform& add_pos(double x, double y, double z) {
		_pos.add_self(x, y, z);
		++_version;
		return *this;
	}

	transform& add_pos(const vec3& delta) {
		_pos += delta;
		++_version;
		return *this;
	}

	transform& set_pos(double x, double y) {
		if (x != _pos.x || y != _pos.z) {
			_pos.add_self(x, 0, y);
			++_version;
		}
		return *this;
	}

	transform& set_pos(const vec3& pos) {
		return set_pos(pos.x, pos.y, pos.z);
	}

	transform& set_pos(double x, double y, double z) {
		if (_pos.x != x || _pos.y != y || _pos.z != z) {
			_pos.x = x;
			_pos.y = y;
			_pos.z = z;
			++_version;
		}
		return *this;
	}

	transform& set_x(double x) {
		if (_pos.x != x) {
			_pos.x = x;
			++_version;
		}
		return *this;
	}

	transform& set_y(double y) {
		if (_pos.y != y) {
			_pos.y = y;
			++_version;
		}
		return *this;
	}

	transform& set_z(double z) {
		if (_pos.z != z) {
			_pos.z = z;
			++_version;
		}
		return *this;
	}

	///////////////////////
	// Scale
	///////////////////////
	double sx() const {
		return _scale.x;
	}

	double sy() const {
		return _scale.y;
	}

	double sz() const {
		return _scale.z;
	}

	const vec3& get_scale() const {
		return _scale;
	}

	transform& set_scale(const vec3& scale) {
		if (_scale != scale) {
			_scale.x = scale.x;
			_scale.y = scale.y;
			_scale.z = scale.z;
			++_version;
		}
		return *this;
	}

	transform& set_scale(double sx, double sy, double sz) {
		if (!_scale.equals(sx, sy, sz)) {
			_scale.x = sx;
			_scale.y = sy;
			_scale.z = sz;
			++_version;
		}
		return *this;
	}

	transform& set_scale(double s) {
		if (!_scale.equals(s, s, s)) {
			_scale = {s, s, s};
			++_version;
		}
		return *this;
	}

	transform& set_scale_x(double sx) {
		if (_scale.x != sx) {
			_scale.x = sx;
			++_version;
		}
		return *this;
	}

	transform& set_scale_y(double sy) {
		if (_scale.y != sy) {
			_scale.y = sy;
			++_version;
		}
		return *this;
	}

	transform& set_scale_z(double sz) {
		if (_scale.z != sz) {
			_scale.z = sz;
			++_version;
		}
		return *this;
	}

	///////////////////////
	// Rotation
	///////////////////////
	transform& set_rotate_x(double degrees) {
		_rotation = set_from_axis_deg(vec3d::X, degrees);
		++_version;
		return *this;
	}

	transform& set_rotate_y(double degrees) {
		_rotation = set_from_axis_deg(vec3d::Y, degrees);
		++_version;
		return *this;
	}

	transform& set_rotate_z(double degrees) {
		_rotation = set_from_axis_deg(vec3d::Z, degrees);
		++_version;
		return *this;
	}

	transform& set_rotate(double x_degrees, double y_degrees, double z_degrees) {
		_rotation = set_euler_angles(x_degrees, y_degrees, z_degrees);
		++_version;
		return *this;
	}

	transform& set_rotate(const quat& quaternion) {
		if (!quaternion.is_exactly(_rotation)) {
			assertion(quaternion.is_normalized(), "Trying to set a non normalized rotation.");
			_rotation = quaternion;
			++_version;
		}
		return *this;
	}

	transform& rotate(const quat& quaternion) {
		assertion(quaternion.is_normalized(), "Trying to rotate with a non normalized rotation.");
		_rotation = _rotation.mul(quaternion);
		++_version;
		return *this;
	}

	const quat& get_rotation() const {
		return _rotation;
	}

	vec3 get_forward() const {
		return rotate_vec3(axis::Forward, get_rotation());
	}

	vec3 get_up() const {
		return rotate_vec3(axis::Up, get_rotation());
	}

	vec3 get_right() const {
		return rotate_vec3(axis::Right, get_rotation());
	}

	transform& set_dir(const vec3& up, const vec3& forward) {
		set_rotate(get_adjusted_rotation(up, forward));
		return *this;
	}

	///////////////////////
	// Matrix
	///////////////////////
	const matrix4& get_matrix() const {
		if (_last_version < _version) {
			_local.set(_pos.x, _pos.y, _pos.z,
			           _rotation.x, _rotation.y, _rotation.z, _rotation.w,
			           _scale.x, _scale.y, _scale.z);
			_last_version = _version;
		}
		return _local;
	}

	///////////////////////
	// Other
	///////////////////////
	bool need_to_recalculate_local() const {
		return _last_version < _version;
	}

	transform& set_to_identity() {
		set_pos(vec3d::zero);
		set_scale(1);
		_rotation = quat::Identity;
		++_version;
		return *this;
	}

	int get_version() const {
		return _version;
	}

private:
	vec3 _pos;
	vec3 _scale = vec3(1, 1, 1);
	quat _rotation;
	int _version = 1;
	mutable int _last_version = 0;
	mutable matrix4 _local;
};
}
