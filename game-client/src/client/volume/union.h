#pragma once

#include "afront/volume.h"

namespace playchilla::volumes {
class union_volume : public volume {
public:
	union_volume(std::vector<const volume*> sources, double min_value = MIN_VALUE) :
		_sources(std::move(sources)),
		_min_value(min_value) {
	}

	const std::vector<const volume*>& get_sources() const {
		return _sources;
	}

	double get_value(double x, double y, double z) const override {
		double max = _min_value;
		for (const auto& source : _sources) {
			max = std::max(max, source->get_value(x, y, z));
		}
		return max;
	}

	void get_data(const vec3& pos, volume_data& data) const override {
		if (const volume* m = _get_volume(pos)) {
			m->get_data(pos, data);
		}
	}

private:
	const volume* _get_volume(const vec3& pos) const {
		const volume* max_module = nullptr;
		double max = _min_value;
		for (const auto* source : _sources) {
			const double value = source->get_value(pos);
			if (value > max) {
				max = value;
				max_module = source;
			}
		}
		return max_module;
	}

	std::vector<const volume*> _sources;
	double _min_value;

	/*
	    void remove(VolumeModule volumeModuleTransformed) {
	        boolean removed = _sources.remove(volumeModuleTransformed);
	        Debug.assertion(removed, "Could not remove entity from physics volume modules.");
	    }
	
	    int getSize() {
	        return _sources.size();
	    }
	    */
};
}
