#pragma once

#include <memory>
#include <queue>

#include "node.h"
#include "core/spatial/point_spatial_hash.h"

namespace playchilla {
using node_hash = point_spatial_hash3<node*>;
using edge_queue = std::deque<edge*>;
using nodes = std::vector<node*>;
using edge_collection = std::vector<std::unique_ptr<edge>>;

class surface_memory {
public:
	surface_memory(double cell_size, class mesh_builder* = nullptr);

	const node_hash& get_node_hash() const;
	size_t get_node_cells() const;
	size_t get_node_count() const;
	size_t get_edge_count() const;
	std::vector<node*> get_nodes(const vec3& pos, double radius) const;
	const edge_queue& get_front() const;

	node* add_node(const vec3& pos, const vec3& normal);
	node* create_node(const vec3& pos, const vec3& normal);
	void remove_node(node*);
	void collapse_node(node*);
	void collapse_nodes_inside(const vec3& center, double radius);
	void collapse_nodes_outside(const vec3& center, double radius);
	void collapse_node_cells_outside(const vec3& center, double radius); // Considers one node per bucket

	edge* push(node*, node*);
	void push(edge*);
	edge* pop_edge();

	void delete_removed();

	void notify_new_triangle(const node*, const node*, const node*, const class volume_data&) const;
	void notify_new_edge(const edge*) const;
	void notify_remove_node(const node*) const;
	void notify_follow_surface_fail() const;

	void validate() const;

private:
	using node_collection = std::vector<std::unique_ptr<node>>;

	node_hash _node_hash;
	mesh_builder* _mesh_builder;
	edge_queue _queue;
	node_collection _nodes;
	edge_collection _edges;
};
}
