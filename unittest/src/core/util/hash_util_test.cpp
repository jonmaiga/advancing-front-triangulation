#include <gtest/gtest.h>

#include "core/util/hash_util.h"
#include "core/util/mx3.h"

namespace playchilla {


TEST(hash, Basic) {
	EXPECT_NE(hash_good(10,0,-1), hash_good(-8,1,4));
	EXPECT_NE(hash_good(0,0,0), hash_good(0,0,1));
	EXPECT_NE(hash_good(0,0,-1), hash_good(0,0,1));
	EXPECT_NE(hash_good(-1,0,0), hash_good(1,0,0));
	EXPECT_NE(hash_good(0,-1,0), hash_good(0,1,0));
	EXPECT_NE(hash_good(-20,0,0), hash_good(-19,0,0));
	EXPECT_NE(hash_good(-20,-20,0), hash_good(-19,-20,0));
	EXPECT_NE(hash_good(0,-20,0), hash_good(0,-19,0));

	std::set<uint64_t> hashes;
	for (int64_t cz = -20; cz <= 20; ++cz) {
		for (int64_t cy = -20; cy <= 20; ++cy) {
			for (int64_t cx = -20; cx <= 20; ++cx) {
				auto h = hash_good(cx, cy, cz);
				if (!hashes.insert(h).second) {
					EXPECT_TRUE(false);
				}
			}
		}
	}
}

TEST(hash, BasicDoubles) {
	EXPECT_NE(hash_double_good(0,0,std::numeric_limits<double>::min()), hash_double_good(0,0,0));
	EXPECT_NE(hash_double_good(0,0,std::numeric_limits<double>::max()), hash_double_good(0,0,0));
	EXPECT_NE(hash_double_good(10.1,0,-1), hash_double_good(-8,1,4));
	EXPECT_NE(hash_double_good(0,0,0.9), hash_double_good(0,0,1));
	EXPECT_NE(hash_double_good(0,0,-1.1), hash_double_good(0,0,1));
	EXPECT_NE(hash_double_good(-1.1,0,0), hash_double_good(1,0,0));
	EXPECT_NE(hash_double_good(0,-1.2,0), hash_double_good(0,1,0));
	EXPECT_NE(hash_double_good(-20.2,0,0), hash_double_good(-19,0,0));
	EXPECT_NE(hash_double_good(-20.1,-20,0), hash_double_good(-19,-20,0));
	EXPECT_NE(hash_double_good(0,-20.1,0), hash_double_good(0,-19.1,0));

	mx3::random r(123);
	std::set<uint64_t> hashes;
	for (int64_t cz = -20; cz <= 20; ++cz) {
		for (int64_t cy = -20; cy <= 20; ++cy) {
			for (int64_t cx = -20; cx <= 20; ++cx) {
				auto h = hash_double_good(cx + 0.1 * r.next_unit(), cy + 0.1 * r.next_unit(), cz + 0.1 * r.next_unit());
				if (!hashes.insert(h).second) {
					EXPECT_TRUE(false);
				}
			}
		}
	}
}

}
