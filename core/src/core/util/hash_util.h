#pragma once

#include <cstdint>

#include "mx3.h"
#include "core/util/conversion.h"

namespace playchilla {
inline uint64_t hash_good(uint64_t x, uint64_t y, uint64_t z) {
	using namespace mx3;

	// very high quality but slow
	x += 0xbea225f9eb34556d;
	y -= 0xbea225f9eb34556d;
	z += 0xe9846af9b1a615d;
	return mix(x ^ mix(y ^ mix(z)));
}

inline uint64_t hash_double_good(double x, double y, double z) {
	return hash_good(util::bits_to<uint64_t>(x), util::bits_to<uint64_t>(y), util::bits_to<uint64_t>(z));
}
}
