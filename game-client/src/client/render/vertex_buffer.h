#pragma once

#include <vector>

#include "core/debug/assertion.h"
#include "core/math/vec3.h"
#include "core/util/rgba.h"

namespace playchilla {
class vertex_buffer {
public:
	std::vector<float> vertices;

	vertex_buffer() = default;

	vertex_buffer(std::size_t capacity) {
		vertices.reserve(capacity);
	}

	void clear() {
		vertices.clear();
	}

	void add(float a) {
		assertion(is_valid(a), "Adding a non valid data to vertex buffer");
		vertices.push_back(a);
	}

	void add(double a) {
		add(static_cast<float>(a));
	}

	void add(double a, double b, double c, double d) {
		add(a);
		add(b);
		add(c);
		add(d);
	}

	void add(const vec3& v) {
		add(v.x);
		add(v.y);
		add(v.z);
	}

	void add(const rgba& c) {
		add(c.r);
		add(c.g);
		add(c.b);
		add(c.a);
	}

	void add(const vec3& a, const vec3& b) {
		add(a);
		add(b);
	}
};
}
