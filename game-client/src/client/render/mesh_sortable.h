#pragma once
#include "core/debug/assertion.h"

namespace playchilla {
class bit_stride {
public:
	bit_stride(std::string name, int start, int bits):
		name(std::move(name)),
		start(start),
		bits(bits),
		stop(start + bits),
		max((1ull << bits) - 1),
		mask(max << start) {
		assertion_2(start + bits <= 63, "Too large mesh sortable 64 for: ", name.c_str());
	}

	bit_stride next(const std::string& in_name, int in_bits) const {
		return {in_name, stop, in_bits};
	}

	uint64_t set(uint64_t data, uint64_t value) const {
		assertion_2(value <= max, "Bit stride value is too large for:", name.c_str());
		data &= ~mask;
		return data + (value << start);
	}

	uint64_t get(uint64_t data) const {
		return (data & mask) >> start;
	}

	std::string name;
	int start;
	int bits;
	int stop;
	uint64_t max;
	uint64_t mask;
};

class mesh_sortable {
public:
	uint64_t get_data() const {
		return _data;
	}

	mesh_sortable& vbo(uint32_t vboId) {
		return _set(_vbo, vboId);
	}

	uint64_t vbo() const {
		return _vbo.get(_data);
	}

	mesh_sortable& shader(uint32_t shaderId) {
		return _set(_shader, shaderId);
	}

	uint64_t shader() const {
		return _shader.get(_data);
	}

	mesh_sortable& depth_back_front(uint64_t depth) {
		return _set(_depth, _depth.max - depth);
	}

	mesh_sortable& depth_front_back(uint64_t depth) {
		return _set(_depth, depth);
	}

	uint64_t depth() const {
		return _depth.get(_data);
	}

	mesh_sortable& blend(bool blend) {
		return _set(_blend, blend ? 1 : 0);
	}

	bool blend() const {
		return _blend.get(_data) == 1;
	}

	mesh_sortable& layer(uint32_t layer) {
		return _set(_layer, layer);
	}

	uint64_t layer() const {
		return _layer.get(_data);
	}

private:
	mesh_sortable& _set(const bit_stride& bit_stride, uint64_t v) {
		_data = bit_stride.set(_data, v);
		return *this;
	}

	inline static const bit_stride _vbo = bit_stride("VBO", 0, 18);
	inline static const bit_stride _shader = _vbo.next("Shader", 9);
	inline static const bit_stride _layer = _shader.next("Layer", 5);
	inline static const bit_stride _depth = _layer.next("Depth", 30);
	inline static const bit_stride _blend = _depth.next("Blend", 1);

	uint64_t _data = 0;
};
}
