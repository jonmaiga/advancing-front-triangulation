#pragma once

#include <vector>
#include "core/math/vec3.h"

namespace playchilla {
class edge;

class node {
public:
	node(const vec3& pos, const vec3& normal);

	node(const node&) = delete;
	node(node&&) = delete;
	node& operator =(const node&) = delete;
	node& operator=(node&&) = delete;
	~node() = default;

	const vec3& get_pos() const;
	bool has_edge_to(const node*) const;
	edge* get_edge_to(const node*) const;
	void mark_for_removal();
	bool is_removed() const;

	vec3 pos;
	vec3 normal;
	std::vector<edge*> edges;

private:
	bool _is_removed = false;
};

const vec3& get_pos(const node*);
}
