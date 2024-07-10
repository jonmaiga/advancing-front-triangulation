#pragma once

#include "core/math/transform.h"
#include "core/util/rgba.h"


namespace playchilla {
class light_view {
public:
	light_view(const rgba& color) : _color(color) {
	}

	const rgba& get_diffuse_color() const {
		return _color;
	}

	const transform& get_transform() const {
		return _transform;
	}

private:
	rgba _color;
	transform _transform;
};
}
