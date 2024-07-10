#pragma once

#include "vbo.h"
#include "vertex_buffer.h"
#include "afront/advancing_front.h"
#include "afront/mesh_builder.h"
#include "client/volume/game_volume_data.h"
#include "core/math/aabb.h"
#include "core/util/rgba.h"

namespace playchilla {
class mesh_builders : public mesh_builder {
public:
	void add(mesh_builder* builder) {
		_builders.push_back(builder);
	}

	void remove(const mesh_builder* builder) {
		_builders.erase(std::find(_builders.begin(), _builders.end(), builder));
	}

	void on_add_triangle(const node* a, const node* b, const node* c, const volume_data& data) override {
		for (auto* builder : _builders) {
			builder->on_add_triangle(a, b, c, data);
		}
	}

	void on_add_edge(const edge* e) override {
		for (auto* builder : _builders) {
			builder->on_add_edge(e);
		}
	}

	void on_remove_node(const node* n) override {
		for (auto* builder : _builders) {
			builder->on_remove_node(n);
		}
	}

private:
	std::vector<mesh_builder*> _builders;
};

class dynamic_vertex_buffer {
public:
	dynamic_vertex_buffer() = default;
	dynamic_vertex_buffer(const dynamic_vertex_buffer&) = delete;
	dynamic_vertex_buffer(dynamic_vertex_buffer&&) = delete;
	dynamic_vertex_buffer& operator=(const dynamic_vertex_buffer&) = delete;
	dynamic_vertex_buffer& operator=(dynamic_vertex_buffer&&) = delete;
	virtual ~dynamic_vertex_buffer() = default;

	virtual bool update_vertex_buffer(vertex_buffer&) = 0;

	void touch() {
		++_version;
	}

	bool is_touched() {
		bool touched = _version != _last_version;
		_last_version = _version;
		return touched;
	}

private:
	uint64_t _version = 0;
	uint64_t _last_version = _version;
};

class normal_mesh_builder : public mesh_builder, public dynamic_vertex_buffer {
public:
	void on_add_triangle(const node* a, const node* b, const node* c, const volume_data& data) override {
		static auto normal_color = rgba(1, 1, 0, 1);
		_add_normals(a, normal_color);
		_add_normals(b, normal_color);
		_add_normals(c, normal_color);
	}

	bool update_vertex_buffer(vertex_buffer& buffer) override {
		std::erase_if(_normals, [](const normal& n) {
			return n.from_node->is_removed();
		});

		buffer.clear();
		for (const auto& n : _normals) {
			buffer.add(n.from_node->get_pos());
			buffer.add(n.color);
			buffer.add(n.to_pos);
			buffer.add(n.color);
		}

		return true;
	}

private:
	void _add_normals(const node* a, const rgba& color) {
		_normals.push_back({a, a->pos + a->normal * .4, color});
	}

	struct normal {
		const node* from_node;
		vec3 to_pos;
		rgba color;
	};

	std::vector<normal> _normals;
};

class vertex_filter {
public:
	vertex_filter() = default;
	vertex_filter(const vertex_filter&) = delete;
	vertex_filter(vertex_filter&&) = delete;
	vertex_filter& operator=(const vertex_filter&) = delete;
	vertex_filter& operator=(vertex_filter&&) = delete;

	virtual ~vertex_filter() = default;
	virtual bool include(const vec3& pos) const = 0;
};

class terrain_mesh_builder : public mesh_builder, public dynamic_vertex_buffer {
public:
	terrain_mesh_builder(const vertex_filter* filter) : _filter(filter) {
	}

	void on_add_triangle(const node* a, const node* b, const node* c, const volume_data& data) override {
		const auto& game_data = std::any_cast<const game_volume_data&>(data.custom_data);
		const auto color = game_data.mat->diffuse_color;
		const vec3 normal = (b->pos - a->pos).cross(c->pos - a->pos).normalize();
		assertion(normal.is_valid(), "Adding invalid normal from triangle");
		_triangles.push_back({a, b, c, normal, color});
	}

	void on_remove_node(const node* n) override {
		// todo: optimize? (can't put it as a single sweep in update_vertex_buffer since memory may be destroyed by then)
		std::erase_if(_triangles, [n](const triangle& t) {
			return t.a == n || t.b == n || t.c == n;
		});
	}

	bool update_vertex_buffer(vertex_buffer& buffer) override {
		buffer.clear();
		for (const auto& t : _triangles) {
			if (_filter) {
				if (!_filter->include(t.a->pos) &&
					!_filter->include(t.b->pos) &&
					!_filter->include(t.c->pos)) {
					continue;
				}
			}
			buffer.add(t.a->pos);
			buffer.add(t.normal);
			buffer.add(t.color);

			buffer.add(t.b->pos);
			buffer.add(t.normal);
			buffer.add(t.color);

			buffer.add(t.c->pos);
			buffer.add(t.normal);
			buffer.add(t.color);
		}
		return true;
	}

private:
	const vertex_filter* _filter;

	struct triangle {
		const node* a;
		const node* b;
		const node* c;
		vec3 normal;
		rgba color;
	};

	std::vector<triangle> _triangles;
};


class static_mesh_builder : public mesh_builder, public dynamic_vertex_buffer {
public:
	void on_add_triangle(const node* a, const node* b, const node* c, const volume_data& data) override {
		const auto& game_data = std::any_cast<const game_volume_data&>(data.custom_data);
		const auto color = game_data.mat->diffuse_color;
		const vec3 normal = (b->pos - a->pos).cross(c->pos - a->pos).normalize();

		_buffer.add(a->pos);
		_buffer.add(normal);
		_buffer.add(color);

		_buffer.add(b->pos);
		_buffer.add(normal);
		_buffer.add(color);

		_buffer.add(c->pos);
		_buffer.add(normal);
		_buffer.add(color);
		touch();
	}

	bool update_vertex_buffer(vertex_buffer& buffer) override {
		if (!is_touched()) {
			return false;
		}
		buffer = _buffer;
		return true;
	}

private:
	vertex_buffer _buffer;
};

class aabb_mesh_builder : public mesh_builder {
public:
	void on_add_triangle(const node* a, const node* b, const node* c, const volume_data& data) override {
		const auto& p1 = a->get_pos();
		const auto& p2 = b->get_pos();
		const auto& p3 = c->get_pos();
		_min = vec3d::min(_min, p1);
		_min = vec3d::min(_min, p2);
		_min = vec3d::min(_min, p3);
		_max = vec3d::max(_max, p1);
		_max = vec3d::max(_max, p2);
		_max = vec3d::max(_max, p3);
	}

	aabb get_aabb() const {
		return aabb::create_from_min_max(_min, _max);
	}

private:
	vec3 _min;
	vec3 _max;
};

class line_mesh_builder : public mesh_builder, dynamic_vertex_buffer {
public:
	void on_add_edge(const edge* e) override {
		_edges.push_back(e);
		touch();
	}

	void on_remove_node(const node* n) override {
		std::erase_if(_edges, [n](const edge* e) {
			return e->a == n || e->b == n;
		});
		touch();
	}

	bool update_vertex_buffer(vertex_buffer& buffer) override {
		if (!is_touched()) {
			return false;
		}
		buffer.clear();
		for (const auto* e : _edges) {
			buffer.add(e->a->get_pos());
			buffer.add(rgba(1, 1, 1, 1));
			buffer.add(e->b->get_pos());
			buffer.add(rgba(1, 1, 1, 1));
		}
		return true;
	}

private:
	std::vector<const edge*> _edges;
};
}
