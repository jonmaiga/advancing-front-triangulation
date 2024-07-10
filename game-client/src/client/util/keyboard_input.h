#pragma once

#include <algorithm>
#include <vector>

#include "core/debug/assertion.h"
#include "core/util/algo.h"

namespace playchilla {
class keyboard_input {
	enum class key_state {
		Pressed,
		Released
	};

	class key_info {
		int _code;
		key_state _state;

	public:
		key_info(int code, key_state state) {
			_code = code;
			_state = state;
		}

		bool is_pressed(int code) const {
			return _code == code && is_pressed();
		}

		bool is_pressed() const {
			return _state == key_state::Pressed;
		}

		bool is_released() const {
			return _state == key_state::Released;
		}

		int code() const {
			return _code;
		}
	};

	static constexpr size_t MaxQueueSize = 64;
	std::vector<key_info> _queue;
	std::vector<int> _down;

public:
	keyboard_input() = default;
	keyboard_input(const keyboard_input&) = delete;
	keyboard_input& operator =(const keyboard_input&) = delete;
	keyboard_input(keyboard_input&&) = delete;
	keyboard_input& operator =(keyboard_input&&) = delete;

	void clear() {
		_queue.clear();
		_down.clear();
	}

	void reset() {
		_queue.clear();
	}

	//////////////////////////////////////////////////
	// Pressed
	//////////////////////////////////////////////////
	void press(int key_code) {
		assertion(_queue.size() < MaxQueueSize, "Keyboard queue (press) is too large, forgot to reset?");
		assertion(_down.size() < MaxQueueSize, "Keyboard down state too large, missing release?");
		_queue.emplace_back(key_code, key_state::Pressed);
		if (!is_down(key_code)) {
			_down.push_back(key_code);
		}
	}

	bool is_pressed(int key_code) const {
		for (auto ki : _queue) {
			if (ki.code() == key_code && ki.is_pressed()) {
				return true;
			}
		}
		return false;
	}

	bool is_any_pressed() const {
		return any_of(_queue, [](const key_info& ki) {
			return ki.is_pressed();
		});
	}

	std::vector<int> get_pressed() const {
		std::vector<int> pressed;
		for (const auto& ki : _queue) {
			if (ki.is_pressed()) {
				pressed.push_back(ki.code());
			}
		}
		return pressed;
	}


	//////////////////////////////////////////////////
	// Released
	//////////////////////////////////////////////////
	void release(int key_code) {
		assertion(_queue.size() < MaxQueueSize, "Keyboard queue (release) is too large, forgot to reset?");
		_queue.emplace_back(key_code, key_state::Released);

		assertion(is_down(key_code), "Releasing a button that is missing in the down state.");
		_down.erase(std::find(_down.begin(), _down.end(), key_code));
	}

	bool is_released(int key_code) const {
		for (auto ki : _queue) {
			if (ki.code() == key_code && ki.is_released()) {
				return true;
			}
		}
		return false;
	}

	//////////////////////////////////////////////////
	// Down
	//////////////////////////////////////////////////
	bool is_down(int key_code) const {
		return std::find(_down.begin(), _down.end(), key_code) != _down.end();
	}

	bool is_any_down() const {
		return !_down.empty();
	}
};
}
