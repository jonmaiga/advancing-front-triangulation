#pragma once

#include <any>
#include <memory>
#include <vector>

#include "game_volume_data.h"
#include "afront/volume_data.h"
#include "client/util/noise/noise.h"
#include "core/debug/assertion.h"
#include "core/math/vec3.h"

namespace playchilla {
class volume_data;

////////////////////////
// LEAFS
////////////////////////
namespace volumes {
class sphere : public volume {
public:
	sphere(double radius) : _radius(radius) {
	}

	double get_value(double x, double y, double z) const override {
		return _radius - std::sqrt(x * x + y * y + z * z);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
	}

private:
	double _radius;
};

class cube : public volume {
public:
	cube(const vec3& size) : _half_extents(size * .5) {
	}

	double get_value(double x, double y, double z) const override {
		const double xa = std::abs(x) - _half_extents.x;
		const double ya = std::abs(y) - _half_extents.y;
		const double za = std::abs(z) - _half_extents.z;
		return -max(xa, max(ya, za));
	}

	void get_data(const vec3& pos, volume_data& data) const override {
	}

private:
	vec3 _half_extents;
};

class constant : public volume {
public:
	constant(double value) : _value(value) {
	}

	double get_value(double x, double y, double z) const override {
		return _value;
	}

	void get_data(const vec3&, volume_data& data) const override {
	}

private:
	double _value;
};

class noise : public volume {
public:
	noise(uint64_t seed, double period) : _noise(seed), _frequency(1. / period) {
	}

	double get_value(double x, double y, double z) const override {
		return _noise.get_value(_frequency * x, _frequency * y, _frequency * z);
	}

	void get_data(const vec3&, volume_data& data) const override {
	}

private:
	gradient_noise _noise;
	double _frequency;
};

class noise2d : public volume {
public:
	noise2d(uint64_t seed, double r, double period) : _noise(seed), _radius(r), _frequency(1. / period) {
		assertion(r > 0, "Sphere radius should be greater than 0");
	}

	double get_value(double x, double y, double z) const override {
		const vec3 surface_pos = vec3{x, y, z}.rescale(_radius);
		return _noise.get_value(_frequency * surface_pos.x, _frequency * surface_pos.y, _frequency * surface_pos.z);
	}

	void get_data(const vec3&, volume_data& data) const override {
	}

private:
	gradient_noise _noise;
	double _radius;
	double _frequency;
};

////////////////////////
// Modifiers
////////////////////////
class negate_value : public volume {
public:
	negate_value(const volume* source) : _source(source) {
	}

	double get_value(double x, double y, double z) const override {
		return -_source->get_value(x, y, z);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		_source->get_data(pos, data);
	}

private:
	const volume* _source;
};

class add_value : public volume {
public:
	add_value(std::vector<const volume*> sources) : _sources(std::move(sources)) {
		assertion(_sources.size() >= 2, "At least two sources are required for add_value");
	}

	double get_value(double x, double y, double z) const override {
		double v = 0;
		for (const auto& s : _sources) {
			v += s->get_value(x, y, z);
		}
		return v;
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		for (const auto& s : _sources) {
			s->get_data(pos, data);
		}
	}

private:
	std::vector<const volume*> _sources;
};

class mul_value : public volume {
public:
	mul_value(std::vector<const volume*> sources) : _sources(std::move(sources)) {
	}

	double get_value(double x, double y, double z) const override {
		double v = 1;
		for (const auto& s : _sources) {
			v *= s->get_value(x, y, z);
		}
		return v;
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		for (const auto& s : _sources) {
			s->get_data(pos, data);
		}
	}

private:
	std::vector<const volume*> _sources;
};

class to_range : public volume {
public:
	to_range(const volume* source, double from, double to) : _source(source), _from(from), _to(to) {
	}

	double get_value(double x, double y, double z) const override {
		const auto v01 = .5 * (1. + _source->get_value(x, y, z));
		assertion(v01 >=0 && v01<=1, "to_range unexpected input");
		return _from + v01 * (_to - _from);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		_source->get_data(pos, data);
	}

private:
	const volume* _source;
	double _from;
	double _to;
};


////////////////////////
// Transformers
////////////////////////
class inv_translate : public volume {
public:
	inv_translate(const volume* source, const volume* x, const volume* y, const volume* z) :
		_source(source),
		_x(x),
		_y(y),
		_z(z) {
	}

	double get_value(double x, double y, double z) const override {
		const double xx = _x->get_value(x, y, z);
		const double yy = _y->get_value(x, y, z);
		const double zz = _z->get_value(x, y, z);
		return _source->get_value(x - xx, y - yy, z - zz);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		const double xx = _x->get_value(pos);
		const double yy = _y->get_value(pos);
		const double zz = _z->get_value(pos);
		_source->get_data(pos - vec3(xx, yy, zz), data);
	}

private:
	const volume* _source;
	const volume* _x;
	const volume* _y;
	const volume* _z;
};

class inv_scale : public volume {
public:
	inv_scale(const volume* source, const volume* s) :
		_source(source),
		_s(s) {
	}

	double get_value(double x, double y, double z) const override {
		const double s = _s->get_value(x, y, z);
		return _source->get_value(x / s, y / s, z / s) * s;
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		const double s = _s->get_value(pos.x, pos.y, pos.z);
		_source->get_data(pos * (1. / s), data);
	}

private:
	const volume* _source;
	const volume* _s;
};

////////////////////////
// DATA
////////////////////////

template <typename T>
class adaptive_edge_len_data : public volume {
public:
	adaptive_edge_len_data(const volume* source, const T& edge_len_calc) :
		_source(source), _edge_len_calc(edge_len_calc) {
	}

	double get_value(double x, double y, double z) const override {
		return _source->get_value(x, y, z);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		data.edge_len = _edge_len_calc(pos);
		return _source->get_data(pos, data);
	}

private:
	const volume* _source;
	T _edge_len_calc;
};

template <typename T>
class set_default_data : public volume {
public:
	set_default_data(const volume* source, T default_data) :
		_source(source), _default_data(default_data) {
	}

	double get_value(double x, double y, double z) const override {
		return _source->get_value(x, y, z);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		data.custom_data = _default_data;
		return _source->get_data(pos, data);
	}

private:
	const volume* _source;
	T _default_data;
};

class set_material_data : public volume {
public:
	set_material_data(const volume* source, const material* material) :
		_source(source),
		_material(material) {
	}

	double get_value(double x, double y, double z) const override {
		return _source->get_value(x, y, z);
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		auto& game_data = std::any_cast<game_volume_data&>(data.custom_data);
		game_data.mat = _material;
		return _source->get_data(pos, data);
	}

private:
	const volume* _source;
	const material* _material;
};
}}
