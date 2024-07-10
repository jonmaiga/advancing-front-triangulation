#pragma once

#include <algorithm>
#include <utility>

#include "difference.h"
#include "fbm.h"
#include "select.h"
#include "union.h"
#include "volumes.h"

namespace playchilla {
class volume_registry {
public:
	template <typename T, typename... Args>
	T* create(Args... args) {
		auto volume = std::make_unique<T>(std::forward<Args>(args)...);
		auto* volume_ptr = volume.get();
		_volumes.push_back(std::move(volume));
		return volume_ptr;
	}

private:
	volume_unique_ptrs _volumes;
};

class csg_instance {
public:
	csg_instance(volume_registry* vr, const volume* current) : _vr(vr), _current(current) {
	}

	//////////////////////////////////////////////////////////////////////
	/// TRANSFORM & SCALE
	////////////////////////////////////////////////////////////////////
	csg_instance& translate(const vec3& t) {
		return create<volumes::inv_translate>(_current, constant(t.x), constant(t.y), constant(t.z));
	}

	csg_instance& scale(double s) {
		const auto* c = constant(s);
		return create<volumes::inv_scale>(_current, c);
	}

	////////////////////////////////////////////////////////////////////
	/// VALUES
	////////////////////////////////////////////////////////////////////
	csg_instance& mul_value(double s) {
		return create<volumes::mul_value>(std::vector{_current, constant(s)});
	}

	csg_instance& fbm(uint32_t octaves = 4) {
		return create<volumes::fbm>(_current, octaves);
	}

	csg_instance& to_range(double from, double to) {
		return create<volumes::to_range>(_current, from, to);
	}

	////////////////////////////////////////////////////////////////////
	/// DATA
	////////////////////////////////////////////////////////////////////
	template <typename T>
	csg_instance& adaptive_edge_len(const T& edge_len_calc) {
		return create<volumes::adaptive_edge_len_data<T>>(_current, edge_len_calc);
	}

	csg_instance& material(const material* material) {
		return create<volumes::set_material_data>(_current, material);
	}

	template <typename T>
	csg_instance& data_type(T default_data) {
		return create<volumes::set_default_data<T>>(_current, default_data);
	}

	////////////////////////////////////////////////////////////////////
	/// OTHER
	////////////////////////////////////////////////////////////////////
	operator const volume*() const {
		return _current;
	}

	const volume* get() const {
		return _current;
	}

	template <typename T, typename... Args>
	csg_instance& create(Args... args) {
		_current = _vr->create<T>(std::forward<Args>(args)...);
		return *this;
	}

private:
	const volume* constant(double c) const {
		return _vr->create<volumes::constant>(c);
	}

	volume_registry* _vr;
	const volume* _current;
};

class csg {
public:
	csg(uint64_t seed) : _vr(std::make_unique<volume_registry>()), _seed(seed), _rnd(seed) {
	}

	csg_instance operator()(const volume* source) const {
		return {_vr.get(), source};
	}

	csg_instance sphere(double radius = .5) {
		return create<volumes::sphere>(radius);
	}

	csg_instance cube(const vec3& size = vec3(1, 1, 1)) {
		return create<volumes::cube>(size);
	}

	csg_instance noise(double period = 1.) {
		return create<volumes::noise>(_seed++, period);
	}

	csg_instance noise_seed(uint64_t seed) {
		return create<volumes::noise>(seed, 1.);
	}

	csg_instance noise_seed(uint64_t seed, double period) {
		return create<volumes::noise>(seed, period);
	}

	csg_instance noise2d(double r, double period = 1.) {
		return create<volumes::noise2d>(_seed++, r, period);
	}

	csg_instance constant(double c) {
		return create<volumes::constant>(c);
	}

	csg_instance add_values(std::vector<const volume*> sources) {
		return create<volumes::add_value>(std::move(sources));
	}

	csg_instance unions(std::vector<const volume*> sources) {
		return create<volumes::union_volume>(std::move(sources));
	}

	csg_instance differences(const volume* lhs, std::vector<const volume*> rhs) {
		return create<volumes::difference>(lhs, std::move(rhs));
	}

	csg_instance select(const volume* first, const volume* second, const volume* controller, volumes::select::condition condition) {
		return create<volumes::select>(first, second, controller, std::move(condition));
	}

	template <typename T, typename... Args>
	csg_instance create(Args... args) {
		return {_vr.get(), _vr->create<T>(std::forward<Args>(args)...)};
	}

	mx3::random& rnd() {
		return _rnd;
	}

private:
	std::unique_ptr<volume_registry> _vr;
	uint64_t _seed;
	mx3::random _rnd;
};

////////////////////////////////////////////////////////////////////
/// SELECT CONDITIONS
////////////////////////////////////////////////////////////////////
inline auto select_greater(double value, double falloff) {
	auto lower = value - falloff;
	auto higher = value + falloff;
	return [lower, inv_interval = 1. / (higher - lower)](double value) {
		return std::clamp(inv_interval * (value - lower), 0., 1.);
	};
}
}
