#include "surface_memory.h"
#include "edge.h"
#include "mesh_builder.h"
#include "node.h"

namespace playchilla {
surface_memory::surface_memory(double cell_size, mesh_builder* mesh_builder) :
	_node_hash(cell_size),
	_mesh_builder(mesh_builder) {
}

const node_hash& surface_memory::get_node_hash() const {
	return _node_hash;
}

size_t surface_memory::get_node_cells() const {
	return _node_hash.get_cell_count();
}

size_t surface_memory::get_node_count() const {
	return _nodes.size();
}

size_t surface_memory::get_edge_count() const {
	return _edges.size();
}

std::vector<node*> surface_memory::get_nodes(const vec3& pos, double radius) const {
	return _node_hash.get_values(pos, radius);
}

const edge_queue& surface_memory::get_front() const {
	return _queue;
}

node* surface_memory::add_node(const vec3& pos, const vec3& normal) {
	return _node_hash.add(create_node(pos, normal));
}

node* surface_memory::create_node(const vec3& pos, const vec3& normal) {
	_nodes.push_back(std::make_unique<node>(pos, normal));
	return _nodes.back().get();
}

edge* surface_memory::push(node* a, node* b) {
	assertion(a != b, "Same edge nodes");
	assertion(!a->has_edge_to(b), "An edge already has a connection to the other node");
	assertion(!b->has_edge_to(a), "An edge already has a connection to the other node");

	_edges.push_back(std::make_unique<edge>(a, b));
	auto* edge = _edges.back().get();
	a->edges.push_back(edge);
	b->edges.push_back(edge);
	_queue.push_back(edge);
	notify_new_edge(edge);
	return edge;
}

void surface_memory::remove_node(node* node) {
	notify_remove_node(node);
	_node_hash.remove(node);
	node->mark_for_removal();
}

void surface_memory::push(edge* edge) {
	assertion(!edge->a->is_removed(), "Adding an edge with removed node");
	assertion(!edge->b->is_removed(), "Adding an edge with removed node");
	assertion(edge->is_used(), "Adding an unused edge.");
	edge->reuse();
	_queue.push_back(edge);
}

edge* surface_memory::pop_edge() {
	if (_queue.empty()) {
		return nullptr;
	}
	auto* e = _queue.front();
	_queue.pop_front();
	return e;
}

void surface_memory::delete_removed() {
	std::erase_if(_queue, [](const edge* e) {
		return e->a->is_removed() || e->b->is_removed();
	});

	std::erase_if(_edges, [](const std::unique_ptr<edge>& e) {
		return e->a->is_removed() || e->b->is_removed();
	});

	std::erase_if(_nodes, [](const std::unique_ptr<node>& n) {
		return n->is_removed();
	});
}

void surface_memory::collapse_node(node* n) {
	assertion(!n->is_removed(), "Node already removed");

	// update neighbors
	for (edge* current_edge : n->edges) {
		current_edge->use();
		node* other = current_edge->get_other(n);
		auto other_edge_it = other->edges.begin();
		while (other_edge_it != other->edges.end()) {
			edge* other_edge = *other_edge_it;
			if (other_edge == current_edge) {
				other_edge_it = other->edges.erase(other_edge_it);
				continue;
			}
			++other_edge_it;
			if (!other_edge->is_used()) {
				continue;
			}
			if (other_edge->get_other(other)->has_edge_to(n)) {
				push(other_edge);
			}
		}
		if (other->edges.empty()) {
			remove_node(other);
		}
	}
	remove_node(n);
}

void surface_memory::collapse_nodes_inside(const vec3& center, double radius) {
	for (node* node : _node_hash.get_values(center, radius)) {
		if (!node->is_removed()) {
			collapse_node(node);
		}
	}
}

void surface_memory::collapse_nodes_outside(const vec3& center, double radius) {
	nodes to_collapse;
	_node_hash.for_each_value([&to_collapse, center, radius_sqr = radius*radius](node* node) {
		if (!node->is_removed() && node->pos.distance_sqr(center) > radius_sqr) {
			to_collapse.push_back(node);
		}
	});
	for (auto* node : to_collapse) {
		if (!node->is_removed()) {
			collapse_node(node);
		}
	}
}

void surface_memory::collapse_node_cells_outside(const vec3& center, double radius) {
	nodes to_collapse;
	_node_hash.for_each_cell([&to_collapse, center, r2 = radius * radius](const nodes& nodes) {
		if (nodes[0]->pos.distance_sqr(center) > r2) {
			to_collapse.insert(to_collapse.begin(), nodes.begin(), nodes.end());
		}
	});

	for (auto* node : to_collapse) {
		if (!node->is_removed()) {
			collapse_node(node);
		}
	}
}

void surface_memory::notify_new_triangle(const node* a, const node* b, const node* c, const volume_data& data) const {
	if (_mesh_builder) {
		_mesh_builder->on_add_triangle(a, b, c, data);
	}
}

void surface_memory::notify_new_edge(const edge* e) const {
	if (_mesh_builder) {
		_mesh_builder->on_add_edge(e);
	}
}

void surface_memory::notify_remove_node(const node* n) const {
	if (_mesh_builder) {
		_mesh_builder->on_remove_node(n);
	}
}

void surface_memory::notify_follow_surface_fail() const {
	if (_mesh_builder) {
		_mesh_builder->inc_follow_surface_fails();
	}
}

void surface_memory::validate() const {
	for (const auto* edge : _queue) {
		assertion(!edge->a->is_removed(), "Validation: An edge in the queue has been removed");
		assertion(!edge->b->is_removed(), "Validation: An edge in the queue has been removed");
	}

	size_t twice_edge_count = 0;
	size_t node_count = 0;
	_node_hash.for_each_value([&](const node* n) {
		assertion(!n->is_removed(), "Validation: Removed node in node hash");
		for (const auto* edge : n->edges) {
			assertion(!edge->a->is_removed(), "Validation: Removed edge node in node hash");
			assertion(!edge->b->is_removed(), "Validation: Removed edge node in node hash");
		}
		twice_edge_count += n->edges.size();
		node_count++;
		
	});

	assertion(2 * get_edge_count() == twice_edge_count, "Validation: Unexpected edge count");
	assertion(get_node_count() == node_count, "Validation: Unexpected node count");
}
}
