#include "advancing_front.h"
#include <array>
#include "volume_util.h"
#include "core/debug/assertion.h"


namespace playchilla {
static const auto TestDirs = std::array{
	vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1),
	vec3(-1, 0, 0), vec3(0, -1, 0), vec3(0, 0, -1)
};

inline vec3 get_perpendicular(const vec3& dir) {
	for (const vec3& test : TestDirs) {
		vec3 p = dir.cross(test);
		if (p.is_near(vec3d::zero)) {
			continue;
		}
		return p.normalize();
	}
	assertion(false, "Didn't find a valid orthogonal vector to normal");
	return vec3d::zero;
}

inline edge* get_common_edge(const edge* e, const node* n) {
	edge* en = e->a->get_edge_to(n);
	return en ? en : e->b->get_edge_to(n);
}

inline size_t count_edges_to(const edge* e, const node* n) {
	return std::count_if(n->edges.begin(), n->edges.end(), [e](const edge* ne) {
		return e->has_common_node(ne);
	});
}

advancing_front::advancing_front(const volume* volume, mesh_builder* mesh_builder, double edge_len, double creation_radius, double error_margin_scale):
	_volume(volume),
	_default_edge_length(edge_len),
	_current_edge_length(edge_len),
	_creation_radius(creation_radius),
	_error_margin_scale(error_margin_scale),
	_surface_memory(15. * edge_len, mesh_builder),
	_data(edge_len) {
	assertion(edge_len > 0, "The edge length should be greater than zero.");
}

advancing_front& advancing_front::ignore_resolution() {
	_use_resolution = false;
	return *this;
}

bool advancing_front::need_seed() const {
	return _surface_memory.get_front().empty() && _surface_memory.get_node_count() == 0;
}

bool advancing_front::try_find_surface(const vec3& search_pos) {
	if (const auto start_surface_pos = find_surface_along_ray(
		_volume,
		search_pos,
		search_pos.normalize(vec3d::X),
		std::max(1., _error_margin_scale * _default_edge_length))) {
		return _create_start_edge(*start_surface_pos);
	}
	return false;
}

double advancing_front::get_edge_length() const {
	return _default_edge_length;
}

double advancing_front::get_creation_radius() const {
	return _creation_radius;
}

const volume* advancing_front::get_volume() const {
	return _volume;
}

surface_memory& advancing_front::get_surface_memory() {
	return _surface_memory;
}

int advancing_front::get_total_steps() const {
	return _total_steps;
}

void advancing_front::build_full_surface(const vec3& generate_pos) {
	step(generate_pos, std::numeric_limits<int>::max());
}

bool advancing_front::step(const vec3& generate_pos, int n) {
	int step = 0;
	int progress = 0;
	edge* stop_edge = nullptr;
	while (step < n) {
		edge* current_edge = _surface_memory.pop_edge();
		if (current_edge == nullptr) {
			break;
		}
		if (current_edge == stop_edge) {
			current_edge->use();
			_surface_memory.push(stop_edge);
			break;
		}
		if (current_edge->is_used()) {
			continue;
		}
		assertion(!current_edge->a->is_removed(), "Removed in system.");
		assertion(!current_edge->b->is_removed(), "Removed in system.");

		current_edge->use();
		++step;
		++_total_steps;

		if (generate_pos.distance_sqr(current_edge->a->get_pos()) >= _creation_radius * _creation_radius) {
			if (stop_edge == nullptr) {
				stop_edge = current_edge;
			}
			_surface_memory.push(current_edge);
			continue;
		}

		++progress;

		const auto maybe_test_pos = _calc_test_pos_follow(current_edge);
		if (!maybe_test_pos) {
			_surface_memory.notify_follow_surface_fail();
			continue;
		}

		const auto& test_pos = *maybe_test_pos;
		if (edge* common_edge = _get_close_with(current_edge, test_pos)) {
			_close_triangle(current_edge, common_edge, common_edge->get_other_node(current_edge));
		}
		else if (node* neighbor = _find_node(current_edge, test_pos)) {
			_triangulate(current_edge, neighbor, get_common_edge(current_edge, neighbor));
		}
	}
	return progress > 0;
}

void advancing_front::_triangulate(const edge* e, node* neighbor, edge* common_edge) {
	if (common_edge == nullptr) {
		_new_triangle(e, neighbor);
	}
	else {
		_close_triangle(e, common_edge, neighbor);
	}
}

void advancing_front::_new_triangle(const edge* edge, node* neighbor) {
	_surface_memory.push(edge->a, neighbor);
	_surface_memory.push(neighbor, edge->b);
	_surface_memory.notify_new_triangle(edge->a, neighbor, edge->b, _data);
}

void advancing_front::_close_triangle(const edge* e, edge* common_edge, node* neighbor) {
	assertion(count_edges_to(e, neighbor) <= 2, "Can max have 2 common connections");

	// todo: this seems a bit wrong, saying an edge is done could leave holes on the other side?
	common_edge->use();
	node* other = e->get_other_node(common_edge);
	edge* old_edge = other->get_edge_to(neighbor);
	if (old_edge != nullptr) {
		old_edge->use();
	}
	else {
		const bool flip = other == e->b;
		_surface_memory.push(flip ? neighbor : other, flip ? other : neighbor);
	}

	_surface_memory.notify_new_triangle(e->a, neighbor, e->b, _data);
}

bool advancing_front::_is_valid(const edge* edge, const node* neighbor) const {
	return _is_valid(edge->a->get_pos(), neighbor->get_pos(), edge->b->get_pos());
}

bool advancing_front::_is_valid(const vec3& a, const vec3& b, const vec3& c) const {
	const vec3& normal = (b - a).cross(c - a).normalize();
	const vec3& center = (a + b + c) * (1. / 3.);
	if (const auto expected = _calc_normal(center)) {
		return normal.dot(*expected) > 0;
	}
	return false;
}

std::optional<vec3> advancing_front::_calc_test_pos_follow(edge* e) {
	return _calc_test_pos_follow(e->a->get_pos(), e->b->get_pos());
}

std::optional<vec3> advancing_front::_calc_test_pos_follow(const vec3& a, const vec3& b) {
	const vec3& align = b - a;
	const vec3& mid_point = align * 0.5 + a;
	_data = {_default_edge_length};
	_volume->get_data(mid_point, _data);
	_current_edge_length = _use_resolution ? _data.edge_len : _default_edge_length;
	return follow_surface(_volume, mid_point, align.normalize(), _error_margin_scale * _current_edge_length, _current_edge_length);
}

bool advancing_front::_create_start_edge(const vec3& start_surface_pos) {
	if (!_surface_memory.get_nodes(start_surface_pos, _current_edge_length).empty()) {
		return false;
	}
	const auto maybe_normal = _calc_normal(start_surface_pos);
	if (!maybe_normal) {
		return false;
	}
	node* a = _surface_memory.add_node(start_surface_pos, *maybe_normal);

	std::optional<vec3> b_pos;
	vec3 dir = get_perpendicular(a->normal);
	assertion(dir.is_valid(), "Didn't find a valid orthogonal vector to normal");
	for (const auto& test_dir : TestDirs) {
		b_pos = follow_surface(_volume, a->get_pos(), dir, _error_margin_scale * _current_edge_length, _current_edge_length);
		if (b_pos) {
			if (_calc_test_pos_follow(a->get_pos(), *b_pos)) {
				break;
			}
		}
		if (1. - std::abs(test_dir.dot(a->normal)) < Epsilon) {
			continue;
		}
		dir = a->normal.cross(test_dir).normalize();
		assertion(dir.is_valid(), "Dir is not valid while finding a start edge");
	}
	if (!b_pos) {
		// todo: remove a?
		return false;
	}

	node* b = _surface_memory.add_node(*b_pos, *_calc_normal(*b_pos));
	_surface_memory.push(a, b);
	return true;
}

edge* advancing_front::_get_close_with(const edge* e, const vec3& test_pos) const {
	edge* best = nullptr;
	double best_alignment = MinAngle;
	std::vector<edge*> edges;
	edges.insert(edges.end(), e->a->edges.begin(), e->a->edges.end());
	edges.insert(edges.end(), e->b->edges.begin(), e->b->edges.end());
	const double e2 = _current_edge_length * _current_edge_length;
	for (edge* close_with : edges) {
		if (close_with == e) {
			continue;
		}

		const node* common = e->get_common_node(close_with);
		const node* neighbor = close_with->get_other(common);
		if (neighbor->get_pos().distance_sqr(test_pos) > e2) {
			continue;
		}

		const vec3& nd = (neighbor->get_pos() - common->get_pos()).normalize();
		const node* other = e->get_other(common);
		const vec3& al = (other->get_pos() - common->get_pos()).normalize();
		const double alignment = al.dot(nd);
		if (alignment < best_alignment) {
			// there is a more narrow angle
			continue;
		}

		if (!_is_valid(e, neighbor)) {
			continue;
		}

		best = close_with;
		best_alignment = alignment;
	}

	return best;
}

node* advancing_front::_find_node(const edge* edge, const vec3& surface_pos) {
	node* closest = nullptr;
	const auto maybe_normal = _calc_normal(surface_pos);
	if (!maybe_normal) {
		return nullptr;
	}
	double closest_dist_sqr = _current_edge_length * _current_edge_length;
	bool found_invalid = false;
	const vec3& normal = *maybe_normal;
	for (node* n : _surface_memory.get_nodes(surface_pos, _current_edge_length)) {
		assertion(n->is_removed() == false, "Removed in spatial");
		if (edge->has_node(n)) {
			continue;
		}

		const double dist_sqr = surface_pos.distance_sqr(n->get_pos());
		if (dist_sqr > closest_dist_sqr) {
			continue;
		}

		if (normal.dot(n->normal) < 0) {
			continue;
		}

		if (!_is_valid(edge, n)) {
			found_invalid = true;
			continue;
		}
		closest = n;
		closest_dist_sqr = dist_sqr;
	}

	if (closest != nullptr) {
		return closest;
	}

	if (found_invalid) {
		return nullptr;
	}

	if (!_is_valid(edge->a->get_pos(), surface_pos, edge->b->get_pos())) {
		return nullptr;
	}

	return _surface_memory.add_node(surface_pos, normal);
}

std::optional<vec3> advancing_front::_calc_normal(const vec3& pos) const {
	return calc_normal(_volume, pos, _current_edge_length);
}
}
