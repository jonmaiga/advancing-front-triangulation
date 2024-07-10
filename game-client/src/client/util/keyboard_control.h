#pragma once

#include "client/util/keyboard_input.h"

namespace playchilla {
struct tick_data;
class transform;

class keyboard_control {
public:
	keyboard_control(const keyboard_input*, transform*);

	void on_tick(const tick_data&) const;

private:
	const keyboard_input* _ki;
	transform* _transform;
};
}
