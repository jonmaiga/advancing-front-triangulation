#pragma once

namespace playchilla {
class node;

class edge {
public:
	edge(node* a, node* b);
	edge(const edge&) = delete;
	edge(edge&&) = delete;
	edge& operator =(const edge&) = delete;
	edge& operator=(edge&&) = delete;
	~edge() = default;

	void use();
	bool is_used() const;
	void reuse();
	bool has_common_node(const edge* e) const;
	const node* get_common_node(const edge* e) const;
	node* get_other(const node* node) const;
	node* get_other_node(const edge* edge) const;
	bool has_node(const node* n) const;

	node* a;
	node* b;

private:
	bool _is_used = false;
};
}
