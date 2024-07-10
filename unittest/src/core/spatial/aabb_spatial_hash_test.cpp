#include <gtest/gtest.h>

#include <algorithm>

#include "core/math/aabb.h"
#include "core/spatial/aabb_spatial_hash.h"

namespace playchilla {
TEST(aabb_spatial_hash, Empty) {
	aabb_spatial_hash sh(10);
	EXPECT_EQ(sh.get_cell_size(), 10);
	EXPECT_EQ(sh.get_values(0, 0, 0).size(), 0);
	EXPECT_EQ(sh.get_values(aabb({100,100,100})).size(), 0);
}

TEST(aabb_spatial_hash, BasicOneCell) {
	aabb_spatial_hash sh(2);
	auto shv = aabb_spatial_hash_value(aabb::create_from_min_max({0, 0, 0}, {1, 1, 1}));
	auto* ptr = &shv;
	sh.add(ptr);
	EXPECT_EQ(sh.get_value_count(), 1);
	EXPECT_EQ(sh.get_cell_count(), 1);
	EXPECT_EQ(sh.get_values(-Epsilon, 0, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, -Epsilon, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, 0, -Epsilon).size(), 0);
	EXPECT_EQ(sh.get_values(0, 0, 0).size(), 1);
	EXPECT_EQ(sh.get_values(0.5, 0.5, 0.5).size(), 1);
	EXPECT_EQ(sh.get_values(1, 1, 1).size(), 1);
	EXPECT_EQ(sh.get_values(1+Epsilon, 0, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, 1+Epsilon, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, 0, 1+Epsilon).size(), 0);

	sh.update(ptr);
	EXPECT_EQ(sh.get_value_count(), 1);
	EXPECT_EQ(sh.get_cell_count(), 1);
	EXPECT_EQ(sh.get_values(-Epsilon, 0, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, -Epsilon, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, 0, -Epsilon).size(), 0);
	EXPECT_EQ(sh.get_values(0, 0, 0).size(), 1);
	EXPECT_EQ(sh.get_values(0.5, 0.5, 0.5).size(), 1);
	EXPECT_EQ(sh.get_values(1, 1, 1).size(), 1);
	EXPECT_EQ(sh.get_values(1+Epsilon, 0, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, 1+Epsilon, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0, 0, 1+Epsilon).size(), 0);

	shv.bb.set_position({1, .5, .5});
	sh.update(ptr);
	EXPECT_EQ(sh.get_value_count(), 1);
	EXPECT_EQ(sh.get_cell_count(), 1);
	EXPECT_EQ(sh.get_values(-Epsilon + 0.5, 0, 0).size(), 0);
	EXPECT_EQ(sh.get_values(0.5, 0, 0).size(), 1);
	EXPECT_EQ(sh.get_values(1., 0.5, 0.5).size(), 1);
	EXPECT_EQ(sh.get_values(1.5, 1, 1).size(), 1);
	EXPECT_EQ(sh.get_values(1.5+Epsilon, 0, 0).size(), 0);

	sh.remove(ptr);
	EXPECT_EQ(sh.get_value_count(), 0);
	EXPECT_EQ(sh.get_cell_count(), 0);
	EXPECT_EQ(sh.get_values(1., 0.5, 0.5).size(), 0);
}

TEST(aabb_spatial_hash, BasicOneNegativeCell) {
	aabb_spatial_hash sh(10);
	auto shv = aabb_spatial_hash_value(aabb::create_from_min_max({-4, -4, -4}, {-2, -2, -2}));
	auto* ptr = &shv;
	sh.add(ptr);
	EXPECT_EQ(sh.get_value_count(), 1);
	EXPECT_EQ(sh.get_cell_count(), 1);
	EXPECT_EQ(sh.get_values(-Epsilon-4, -4, -4).size(), 0);
	EXPECT_EQ(sh.get_values(-4, -Epsilon-4, -4).size(), 0);
	EXPECT_EQ(sh.get_values(-4, -4, -Epsilon-4).size(), 0);
	EXPECT_EQ(sh.get_values(-4, -4, -4).size(), 1);
	EXPECT_EQ(sh.get_values(-3, -3, -3).size(), 1);
	EXPECT_EQ(sh.get_values(-2, -2, -2).size(), 1);
	EXPECT_EQ(sh.get_values(-2+Epsilon, -2, -2).size(), 0);
	EXPECT_EQ(sh.get_values(-2, -2+Epsilon, -2).size(), 0);
	EXPECT_EQ(sh.get_values(-2, -2, -2+Epsilon).size(), 0);

	sh.remove(ptr);
	EXPECT_EQ(sh.get_value_count(), 0);
	EXPECT_EQ(sh.get_cell_count(), 0);
	EXPECT_EQ(sh.get_values(1., 0.5, 0.5).size(), 0);
}

TEST(aabb_spatial_hash, BasicMultipleCells) {
	aabb_spatial_hash sh(1);
	auto shv = aabb_spatial_hash_value(aabb::create_from_min_max({0, 0, 0}, {1, 1, 1}));
	auto* ptr = &shv;
	sh.add(ptr);
	EXPECT_EQ(sh.get_value_count(), 8);
	EXPECT_EQ(sh.get_cell_count(), 8);
	auto bb = aabb::create_from_min_max({-1, -1, -1}, {-.99, -.99, -99});
	EXPECT_EQ(sh.get_values(bb).size(), 0);
	bb = aabb::create_from_min_max({-1, -1, -1}, {2, 2, 2});
	EXPECT_EQ(sh.get_values(bb).size(), 1);
	bb = aabb::create_from_min_max({.99, .99, .99}, {2, 2, 2});
	EXPECT_EQ(sh.get_values(bb).size(), 1);

	shv.bb.set_position({10, 10, 10});
	sh.update(ptr);
	EXPECT_EQ(sh.get_value_count(), 8);
	EXPECT_EQ(sh.get_cell_count(), 8);
	bb = aabb::create_from_min_max({-1, -1, -1}, {2, 2, 2});
	EXPECT_EQ(sh.get_values(bb).size(), 0);
	bb = aabb::create_from_min_max({8, 8, 8}, {9.4, 9.4, 9.4});
	EXPECT_EQ(sh.get_values(bb).size(), 0);
	bb = aabb::create_from_min_max({8, 8, 8}, {9.51, 9.51, 9.51});
	EXPECT_EQ(sh.get_values(bb).size(), 1);

	sh.remove(ptr);
	EXPECT_EQ(sh.get_value_count(), 0);
	EXPECT_EQ(sh.get_cell_count(), 0);
	EXPECT_EQ(sh.get_values(1., 0.5, 0.5).size(), 0);
	bb = aabb::create_from_min_max({8, 8, 8}, {9.51, 9.51, 9.51});
	EXPECT_EQ(sh.get_values(bb).size(), 0);
}

aabb get_random_aabb(mx3::random& r) {
	return aabb(
		{
			r.between(-20, 20),
			r.between(-10, 10),
			r.between(-20, 20)
		},
		{
			r.between(1, 30),
			r.between(1, 30),
			r.between(1, 30)
		});
}

TEST(aabb_spatial_hash, RandomAabb) {
	aabb_spatial_hash sh(5);

	mx3::random r(123);
	std::vector<std::unique_ptr<aabb_spatial_hash_value>> all;
	for (int i = 0; i < 1000; ++i) {
		all.emplace_back(std::make_unique<aabb_spatial_hash_value>(get_random_aabb(r)));
	}

	for (const auto& shv : all) {
		sh.add(shv.get());
	}

	auto get_overlapping = [&all](const aabb& bb) {
		std::vector<aabb_spatial_hash_value*> result;
		for (const auto& shv : all) {
			if (shv->overlaps(bb)) {
				result.push_back(shv.get());
			}
		}
		return result;
	};

	auto verify = [&] {
		for (int i = 0; i < 1000; ++i) {
			auto bb = get_random_aabb(r);
			auto r1 = sh.get_values(bb);
			auto r2 = get_overlapping(bb);
			std::sort(r1.begin(), r1.end());
			std::sort(r2.begin(), r2.end());
			if (r1 != r2) {
				return false;
			}
		}
		return true;
	};
	EXPECT_TRUE(verify());

	for (const auto& shv : all) {
		auto p = shv->bb.get_center();
		shv->bb.set_position(p + vec3d::create_random_dir(r).scale(10 * r.next_unit()));
		sh.update(shv.get());
	}

	EXPECT_TRUE(verify());

	for (auto it = all.begin(); it != all.end(); ++it) {
		if (r.next_unit() < 0.5) {
			sh.remove(it->get());
			it = all.erase(it);
		}
	}

	EXPECT_TRUE(verify());

	for (auto& shv : all) {
		sh.remove(shv.get());
	}
	all.clear();

	EXPECT_TRUE(verify());
}
}
