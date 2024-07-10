#include "node.h"

#include "edge.h"
#include "core/debug/assertion.h"

namespace playchilla {
node::node(const vec3& pos, const vec3& normal): pos(pos), normal(normal) {
	assertion(pos.is_valid(), "Node pos is invalid");
	assertion(normal.is_valid(), "Node normal is invalid");
}

const vec3& node::get_pos() const {
	return pos;
}

bool node::has_edge_to(const node* n) const {
	return get_edge_to(n) != nullptr;
}

edge* node::get_edge_to(const node* n) const {
	assertion(n != this, "Checking if a node has an edge to self.");
	for (auto* e : edges) {
		if (e->has_node(n)) {
			return e;
		}
	}
	return nullptr;
}

void node::mark_for_removal() {
	edges.clear();
	_is_removed = true;
}

bool node::is_removed() const {
	return _is_removed;
}

const vec3& get_pos(const node* n) {
	return n->pos;
}
}
