#pragma once

#include "render/mesh_builders.h"

namespace playchilla {
class surface_entity {
public:
	surface_entity(const volume* volume, const transform* update_around, double edge_len = 1.) :
		_vbo(_vertices.vertices),
		_view([this] { return &_vbo; }, shader_type::line_shader, GL_LINES),
		_advancing_front(volume, &_mesh_builder, edge_len, 100., 0.05),
		_update_around(update_around) {
	}

	void on_tick(const tick_data&) {
		_advancing_front.get_surface_memory().delete_removed();

		const auto& local_update_pos = _update_around->get_pos() - _view.get_transform().get_pos();
		if (_advancing_front.need_seed()) {
			_try_setup_surface_position(local_update_pos);
		}

		_advancing_front.step(local_update_pos, 100);

		if (_vbo.is_ready_to_upload()) return;

		_vbo.mark_for_processing();
		_mesh_builder.update_vertex_buffer(_vertices);
		_vbo.mark_for_upload();

		_advancing_front.get_surface_memory().collapse_nodes_outside(local_update_pos, _advancing_front.get_creation_radius() * 1.2);
	}

	const mesh_view* get_view() const {
		return &_view;
	}

	mesh_view* get_view() {
		return &_view;
	}

private:
	bool _try_setup_surface_position(const vec3& around_position) {
		const double creation_radius = _advancing_front.get_creation_radius();
		const auto distance = std::abs(_advancing_front.get_volume()->get_value(around_position));
		if (distance >= creation_radius) {
			return false;
		}

		if (!around_position.is_near({0, 0, 0})) {
			if (_advancing_front.try_find_surface(around_position)) {
				return true;
			}
		}

		return _advancing_front.try_find_surface({0.1, 0.2, -0.05});
	}

	line_mesh_builder _mesh_builder;
	vertex_buffer _vertices;
	vbo _vbo;
	mesh_view _view;
	advancing_front _advancing_front;
	const transform* _update_around;
};
}
