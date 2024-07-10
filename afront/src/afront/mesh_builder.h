#pragma once

namespace playchilla {
class node;
class volume_data;

class mesh_builder {
public:
	mesh_builder() = default;
	mesh_builder(const mesh_builder&) = delete;
	mesh_builder(mesh_builder&&) = delete;
	mesh_builder& operator=(const mesh_builder&) = delete;
	mesh_builder& operator=(mesh_builder&&) = delete;
	virtual ~mesh_builder() = default;

	virtual void on_add_triangle(const node*, const node*, const node*, const volume_data&) {
	}

	virtual void on_add_edge(const edge*) {
	}

	virtual void on_remove_node(const node*) {
	}

	virtual void inc_follow_surface_fails() {
	}
};
}
