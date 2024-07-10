#include <gtest/gtest.h>

#include "core/math/math_util.h"
#include "core/util/mx3.h"

namespace playchilla {


TEST(math, MinMax) {
	EXPECT_EQ(max(1., .1 - std::numeric_limits<double>::min()), 1);
	EXPECT_EQ(min(1., .1 - std::numeric_limits<double>::min()), .1 - std::numeric_limits<double>::min());
	mx3::random rnd(13);
	for (int i = 0; i < 1000; ++i) {
		double a = i * rnd.next_unit();
		double b = i * rnd.next_unit();
		EXPECT_EQ(min(a,b), std::min(a,b));
		EXPECT_EQ(max(a,b), std::max(a,b));
	}
}

}
