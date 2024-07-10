#pragma once

#include "client/render/mesh_sortable.h"
#include "client/render/render_setting.h"
#include "client/render/vbo.h"
#include "client/render/shader/shader_map.h"
#include "core/math/matrix4.h"
#include "core/math/transform.h"

namespace playchilla {
class mesh_view {
public:
	mesh_view(const std::function<vbo*()>& vbo_provider, shader_type shader_type, GLenum draw_mode) :
		_shader_type(shader_type),
		_vbo_provider(vbo_provider) {
		_render_setting.draw_mode = draw_mode;
		_render_setting.buffer_id = vbo_provider()->get_id();
		_sortable.shader(static_cast<uint32_t>(_shader_type));
	}

	bool has_vbo_data() const {
		return get_vbo()->get_buffer_byte_size() > 0;
	}

	vbo* get_vbo() const {
		return _vbo_provider();
	}

	shader_type get_shader_type() const {
		return _shader_type;
	}

	const render_setting& get_render_settings() const {
		return _render_setting;
	}

	void set_cull(bool enable) {
		_render_setting.is_cull = enable;
	}

	uint64_t get_sort_id() const {
		return _sortable.get_data();
	}

	void set_depth_test(bool enable) {
		_render_setting.depth_test = enable;
	}

	void set_depth_write(bool enable) {
		_render_setting.depth_write = enable;
	}

	void set_stencil_test(bool enable) {
		_render_setting.stencil_test = enable;
	}

	void set_blend(bool blend) {
		_render_setting.blend = blend;
		_sortable.blend(blend);
	}

	void set_blend_func(int src_blend, int dst_blend) {
		_render_setting.src_blend = src_blend;
		_render_setting.dst_blend = dst_blend;
	}

	void set_render_property(const std::string& key, double value) {
		assertion_2(!_render_properties.contains(key), "Render property already added: ", key.c_str());
		_render_properties[key] = value;
	}

	double get_render_property(const std::string& key) const {
		assertion_2(_render_properties.contains(key), "Could not find render property: ", key.c_str());
		return _render_properties.find(key)->second;
	}

	transform& get_transform() {
		return _transform;
	}

	const transform& get_transform() const {
		return _transform;
	}

private:
	shader_type _shader_type;
	std::function<vbo*()> _vbo_provider;
	render_setting _render_setting;
	mesh_sortable _sortable;
	std::map<std::string, double> _render_properties;
	transform _transform;
};
}
