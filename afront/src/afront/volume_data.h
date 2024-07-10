#pragma once

#include <any>

namespace playchilla {
class volume_data {
public:
	volume_data(double edge_len) : edge_len(edge_len) {
	}

	double edge_len;

	std::any custom_data;
};
}
