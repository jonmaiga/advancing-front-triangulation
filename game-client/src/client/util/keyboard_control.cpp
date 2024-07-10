#include "keyboard_control.h"

#include "client/util/keyboard_input.h"
#include "core/math/quat_util.h"
#include "core/math/transform.h"
#include "GLFW/glfw3.h"

namespace playchilla {
keyboard_control::keyboard_control(const keyboard_input* ki, transform* transform) : _ki(ki), _transform(transform) {
}

void keyboard_control::on_tick(const tick_data&) const {

	constexpr double turn_degrees = 4;

	auto& ki = *_ki;

	// ROLL
	if (ki.is_down(GLFW_KEY_Q)) {
		_transform->rotate(set_from_axis_deg(axis::Forward, -turn_degrees));
	}
	else if (ki.is_down(GLFW_KEY_E)) {
		_transform->rotate(set_from_axis_deg(axis::Forward, turn_degrees));
	}

	// ROTATE LEFT & RIGHT
	if (ki.is_down(GLFW_KEY_LEFT)) {
		_transform->rotate(set_from_axis_deg(axis::Up, turn_degrees));
	}
	else if (ki.is_down(GLFW_KEY_RIGHT)) {
		_transform->rotate(set_from_axis_deg(axis::Up, -turn_degrees));
	}

	// ROTATE UP & DOWN
	if (ki.is_down(GLFW_KEY_UP)) {
		_transform->rotate(set_from_axis_deg(axis::Right, -turn_degrees));
	}
	else if (ki.is_down(GLFW_KEY_DOWN)) {
		_transform->rotate(set_from_axis_deg(axis::Right, turn_degrees));
	}

	vec3 _delta_move{};
	const vec3& dir = _transform->get_forward();
	const vec3& up = _transform->get_up();
	const vec3& right = _transform->get_right();
	// MOVE FORWARD & BACKWARD
	if (ki.is_down(GLFW_KEY_W)) {
		_delta_move.add_self(dir.x, dir.y, dir.z);
	}
	else if (ki.is_down(GLFW_KEY_S)) {
		_delta_move.add_self(-dir.x, -dir.y, -dir.z);
	}

	// MOVE UP & DOWN
	if (ki.is_down(GLFW_KEY_SPACE)) {
		_delta_move.add_self(up.x, up.y, up.z);
	}
	else if (ki.is_down(GLFW_KEY_LEFT_SHIFT)) {
		_delta_move.sub_self(up.x, up.y, up.z);
	}

	// STRAFE LEFT & RIGHT
	if (ki.is_down(GLFW_KEY_A)) {
		_delta_move.add_self(-right.x, -right.y, -right.z);
	}
	else if (ki.is_down(GLFW_KEY_D)) {
		_delta_move.add_self(right.x, right.y, right.z);
	}

	if (_delta_move.length_sqr() > EpsilonSqr) {
		_delta_move.normalize_self();
		if (ki.is_down(GLFW_KEY_RIGHT_CONTROL)) {
			_delta_move = _delta_move.rescale(10);
		}
		if (ki.is_down(GLFW_KEY_RIGHT_ALT)) {
			_delta_move = _delta_move.rescale(100);
		}
	}
	else {
		_delta_move = {};
	}

	if (ki.is_down(GLFW_KEY_BACKSPACE)) {
		_delta_move *= 1000.;
	}
	if (ki.is_down(GLFW_KEY_RIGHT_SHIFT)) {
		_delta_move *= 0.05;
	}

	_transform->set_pos(_transform->get_pos() + _delta_move);
}
}
