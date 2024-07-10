#pragma once

#include "edge.h"
#include "surface_memory.h"
#include "volume_data.h"
#include "volume.h"

namespace playchilla {

class advancing_front {
public:
	advancing_front(const volume*, mesh_builder*, double edge_len, double creation_radius, double error_margin_scale = 0.1);

	advancing_front& ignore_resolution();
	bool need_seed() const;
	bool try_find_surface(const vec3& search_pos);
	
	double get_edge_length() const;
	double get_creation_radius() const;
	const volume* get_volume() const;
	surface_memory& get_surface_memory();
	int get_total_steps() const;

	void build_full_surface(const vec3& generate_pos);
	bool step(const vec3& generate_pos, int n);

private:
	void _triangulate(const edge* e, node* neighbor, edge* common_edge);
	void _new_triangle(const edge* edge, node* neighbor);
	void _close_triangle(const edge* e, edge* common_edge, node* neighbor);
	bool _is_valid(const edge* edge, const node* neighbor) const;
	bool _is_valid(const vec3& a, const vec3& b, const vec3& c) const;
	std::optional<vec3> _calc_test_pos_follow(edge*);
	std::optional<vec3> _calc_test_pos_follow(const vec3& a, const vec3& b);
	bool _create_start_edge(const vec3& start_surface_pos);
	edge* _get_close_with(const edge* e, const vec3& test_pos) const;
	node* _find_node(const edge* edge, const vec3& surface_pos);
	std::optional<vec3> _calc_normal(const vec3& pos) const;

	inline static const auto MinAngle = std::cos(deg_to_rad(93));
	const volume* _volume;
	double _default_edge_length;
	double _current_edge_length;
	double _creation_radius;
	double _error_margin_scale;

	surface_memory _surface_memory;
	volume_data _data;
	int _total_steps = 0;
	bool _use_resolution = true;
};
}
