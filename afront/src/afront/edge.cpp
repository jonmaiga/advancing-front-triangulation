#include "edge.h"
#include "core/debug/assertion.h"


namespace playchilla {
edge::edge(node* a, node* b) : a(a), b(b) {
}

void edge::use() {
	_is_used = true;
}

bool edge::is_used() const {
	return _is_used;
}

void edge::reuse() {
	assertion(is_used(), "Trying to reuse an unused edge.");
	_is_used = false;
}

bool edge::has_common_node(const edge* e) const {
	return get_common_node(e) != nullptr;
}

const node* edge::get_common_node(const edge* e) const {
	if (a == e->a || a == e->b) {
		return a;
	}
	if (b == e->a || b == e->b) {
		return b;
	}
	return nullptr;
}

node* edge::get_other(const node* node) const {
	if (node == a) {
		return b;
	}
	if (node == b) {
		return a;
	}
	assertion(false, "Node not in edge");
	return nullptr;
}

node* edge::get_other_node(const edge* edge) const {
	return get_other(get_common_node(edge));
}

bool edge::has_node(const node* n) const {
	return a == n || b == n;
}
}
