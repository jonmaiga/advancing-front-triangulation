#pragma once

namespace playchilla::volumes {
class fbm : public volume {
public:
	fbm(const volume* source, uint32_t octaves) : _source(source), _octaves(octaves) {
		double amp = _gain;
		double ampFractal = 1.0f;
		for (uint32_t i = 1; i < _octaves; i++) {
			ampFractal += 0.5 * amp;
			amp *= _gain;
		}
		_scale = 1.0 / ampFractal;
	}


	double get_value(double x, double y, double z) const override {
		double sum = 0;
		double amp = 1.0;
		for (uint32_t i = 0; i < _octaves; ++i) {
			sum += amp * _source->get_value(x, y, z);
			x *= _lacunarity;
			y *= _lacunarity;
			z *= _lacunarity;
			amp *= _gain;
		}
		return _scale * sum;
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		_source->get_data(pos, data);
	}

private:
	const volume* _source;
	double _scale;
	uint32_t _octaves;
	double _lacunarity = 2.;
	double _gain = 0.5;
};
}
