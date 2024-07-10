#pragma once

namespace util {

template <typename Target, typename Source>
Target bits_to(Source v) {
	static_assert(sizeof(Target) == sizeof(Source), "Expected target type to be the same size as the value size");
	return *reinterpret_cast<Target*>(&v);
}

}
