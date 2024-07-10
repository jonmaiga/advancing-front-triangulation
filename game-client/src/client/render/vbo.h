#pragma once

#include <functional>
#include <vector>

#include <core/debug/assertion.h>
#include "xgl/xgl.h"

namespace playchilla {
using vbo_callback = std::function<void(const class vbo&)>;

class vbo final {
public:
	vbo(std::vector<float>& data) : _data(data) {
		assertion(_data.empty(), "Expected initial empty data");
		gl_check(glGenBuffers(1, &_id));
	}

	vbo(const vbo&) = delete;
	vbo& operator=(const vbo&) = delete;
	vbo(vbo&&) = delete;
	vbo& operator=(vbo&&) = delete;

	~vbo() {
		gl_check(glDeleteBuffers(1, &_id));
	}

	void try_upload() {
		if (!is_ready_to_upload()) {
			return;
		}

		if (!_data.empty()) {
			assertion(is_ready_to_upload(), "Expected ready to upload before getting uploaded");

			const auto capacity_bytes = 4 * _data.capacity();
			if (capacity_bytes != _last_gpu_capacity_bytes) {
				_allocate(capacity_bytes);
			}
			gl_check(glBufferSubData(GL_ARRAY_BUFFER, 0, capacity_bytes, _data.data()));
		}
		_state = state::uploaded;
	}

	GLsizeiptr get_buffer_byte_size() const {
		return 4 * static_cast<GLsizeiptr>(_data.size());
	}

	GLuint get_id() const {
		return _id;
	}

	void mark_for_processing() {
		assertion(is_processing() || is_uploaded(), "Expeced VBO state to already be in 'processing' or 'uploaded");
		_state = state::processing;
	}

	void mark_for_upload() {
		assertion(is_processing(), "Expected VBO state to be 'processing' before setting to 'ready to upload'");
		_state = state::ready_to_upload;
	}

	bool is_processing() const {
		return _state == state::processing;
	}

	bool is_ready_to_upload() const {
		return _state == state::ready_to_upload;
	}

	bool is_uploaded() const {
		return _state == state::uploaded;
	}

	std::vector<float>& get_data() const {
		return _data;
	}

private:
	void _allocate(std::size_t capacity_bytes) {
		gl_check(glBufferData(GL_ARRAY_BUFFER, capacity_bytes, nullptr, GL_DYNAMIC_DRAW));
		_last_gpu_capacity_bytes = static_cast<GLsizeiptr>(capacity_bytes);
	}

	enum class state {
		processing,
		ready_to_upload,
		uploaded
	};

	state _state = state::processing;
	GLsizeiptr _last_gpu_capacity_bytes = {};

	GLuint _id{};
	std::vector<float>& _data;
};
}
