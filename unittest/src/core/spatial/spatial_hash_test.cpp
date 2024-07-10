#include <algorithm>
#include <gtest/gtest.h>

#include "core/util/mx3.h"
#include "core/spatial/spatial_hash.h"

namespace playchilla {

TEST(spatial_hash, Empty) {
	const spatial_hash<int> sh(10);
	EXPECT_EQ(sh.get_cell_size(), 10);
	EXPECT_EQ(sh.get_cell_count(), 0);
	EXPECT_EQ(sh.get_value_count(), 0);
	EXPECT_TRUE(sh.get_cell_center(vec3(3,3,3)).is_near(5,5,5));
	EXPECT_TRUE(sh.get_cell_center(vec3(-7,-7,-7)).is_near(-5,-5,-5));
	EXPECT_TRUE(sh.get_cell_center(vec3(-7,7,-7)).is_near(-5,5,-5));
	EXPECT_TRUE(sh.get_cell_center(vec3(13,13,13)).is_near(15,15,15));
	EXPECT_TRUE(sh.get_cell_values({}).empty());
}

TEST(spatial_hash, Basic) {
	spatial_hash<int> sh(10);
	sh.add_to_cell(1, vec3(3, 3, 3));
	sh.add_to_cell(2, vec3(3, 3, 9.9));

	sh.add_to_cell(3, vec3(-1, 3, 9.9));
	sh.add_to_cell(4, vec3(-9.9, 3, 9.9));

	sh.add_to_cell(5, vec3(3, 3, 13));
	sh.add_to_cell(6, vec3(-11, 1, 1));

	EXPECT_EQ(sh.get_value_count(), 6);
	EXPECT_EQ(sh.get_cell_count(), 4);

	EXPECT_EQ(sh.get_cell_values(vec3(5,5,5)), (std::vector<int>{1, 2}));
	EXPECT_EQ(sh.get_cell_values(vec3(-5,5,5)), (std::vector<int>{3, 4}));

	EXPECT_EQ(sh.get_cell_values(vec3(5,5,15)), (std::vector<int>{5}));
	EXPECT_EQ(sh.get_cell_values(vec3(-15,5,5)), (std::vector<int>{6}));

	sh.remove_from_cell(1, vec3(3, 3, 3));
	EXPECT_EQ(sh.get_cell_values(vec3(5,5,5)), (std::vector<int>{2}));
	EXPECT_EQ(sh.get_cell_count(), 4);

	sh.remove_from_cell(5, vec3(3, 3, 13));
	EXPECT_TRUE(sh.get_cell_values(vec3(3,3,13)).empty());
	EXPECT_EQ(sh.get_cell_count(), 3);
}

class spatial_hash_edge_test : public ::testing::TestWithParam<double> {
};

TEST_P(spatial_hash_edge_test, EdgeCase) {
	double s = GetParam();
	spatial_hash<int> sh(s);
	sh.add_to_cell(-1, vec3(-s, 0, 0)); // goes into [-s,0)
	sh.add_to_cell(0, vec3(0, 0, 0)); // goes into [0,s)
	sh.add_to_cell(1, vec3(s, 0, 0)); // goes into [s,2s)
	EXPECT_EQ(sh.get_cell_values({-s-Epsilon, 0, 0}), (std::vector<int>{}));
	EXPECT_EQ(sh.get_cell_values({-s, 0, 0}), (std::vector<int>{-1}));
	EXPECT_EQ(sh.get_cell_values({-Epsilon, 0, 0}), (std::vector<int>{-1}));
	EXPECT_EQ(sh.get_cell_values({0, 0, 0}), (std::vector<int>{0}));
	EXPECT_EQ(sh.get_cell_values({s-Epsilon, 0, 0}), (std::vector<int>{0}));
	EXPECT_EQ(sh.get_cell_values({s, 0, 0}), (std::vector<int>{1}));
	EXPECT_EQ(sh.get_cell_values({2*s-Epsilon, 0, 0}), (std::vector<int>{1}));
	EXPECT_EQ(sh.get_cell_values({2*s, 0, 0}), (std::vector<int>{}));
}

INSTANTIATE_TEST_SUITE_P(SpatialHashInstance, spatial_hash_edge_test, ::testing::Values(1, 2, 1e7));


TEST(spatial_hash, Stress) {
	spatial_hash<int> sh(10);
	mx3::random rnd(123);
	for (int i = 0; i < 1000; ++i) {
		const auto pred = [i](int j) { return i == j; };
		const auto pos = vec3(rnd.next_unit() * 100, rnd.next_unit() * 100, rnd.next_unit() * 100);
		sh.add_to_cell(i, pos);
		auto vs = sh.get_cell_values(pos);
		EXPECT_TRUE(std::any_of(vs.begin(), vs.end(), pred));
		if (i % 5 == 0) {
			sh.remove_from_cell(i, pos);
			auto vs2 = sh.get_cell_values(pos);
			EXPECT_FALSE(std::any_of(vs2.begin(), vs2.end(), pred));
			EXPECT_TRUE(vs2.size() == vs.size()-1);
		}
	}
}

}
