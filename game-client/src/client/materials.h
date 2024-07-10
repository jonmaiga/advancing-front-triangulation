#pragma once

#include <map>
#include <memory>

#include "core/debug/assertion.h"
#include "core/util/rgba.h"

namespace playchilla {
enum class material_type {
	grass,
	plains,
	rock,
	sun,
	unset,
};

struct material {
	material_type type;
	rgba diffuse_color;
};

class material_registry {
public:
	material_registry() {
		_register({material_type::grass, rgba{0.88, 0.85, 0.63}});
		_register({material_type::plains, rgba{0.12, 0.41, 0.1}});
		_register({material_type::rock, rgba{0.31, 0.77, 0.63}});
		_register({material_type::sun, rgba{1., 0.96, 0.}});
		_register({material_type::unset, rgba{1., 0, 0}});
	}

	const material* grass() const {
		return _get(material_type::grass);
	}

	const material* plains() const {
		return _get(material_type::plains);
	}

	const material* rock() const {
		return _get(material_type::rock);
	}

	const material* sun() const {
		return _get(material_type::sun);
	}

	const material* unset() const {
		return _get(material_type::unset);
	}

private:
	void _register(material new_material) {
		assertion(_materials.find(new_material.type) == _materials.end(), "Material already registered");
		_materials[new_material.type] = std::make_unique<material>(new_material);
	}

	const material* _get(material_type type) const {
		return _materials.find(type)->second.get();
	}

	std::map<material_type, std::unique_ptr<const material>> _materials;
};

inline const material_registry& get_material_registry() {
	static const material_registry materials;
	return materials;
}
}
