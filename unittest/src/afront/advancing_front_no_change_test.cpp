#include <gtest/gtest.h>

#include "mesh_inspector.h"
#include "afront/advancing_front.h"
#include "afront/mesh_builder.h"
#include "client/volume/csg.h"
#include "core/util/timer.h"
#include "test_models.h"


namespace playchilla {
class debug_mesh_builder : public mesh_builder {
public:
	void on_add_triangle(const node* a, const node* b, const node* c, const volume_data& d) override {
		hash ^= hash_double_good(a->pos.x, a->pos.y, a->pos.z);
		hash ^= hash_double_good(b->pos.x, b->pos.y, b->pos.z);
		hash ^= hash_double_good(c->pos.x, c->pos.y, c->pos.z);
		hash ^= hash_double_good(a->normal.x, a->normal.y, a->normal.z);
		hash ^= hash_double_good(b->normal.x, b->normal.y, b->normal.z);
		hash ^= hash_double_good(c->normal.x, c->normal.y, c->normal.z);
		triangles.emplace_back(a->pos, b->pos, c->pos);
	}

	void inc_follow_surface_fails() override {
		++failed_follows;
	}

	uint64_t hash = 0;
	uint64_t failed_follows = 0;
	std::vector<test_triangle> triangles;
};

TEST(advancing_front_no_change, UnitSphere) {
	csg csg(12345);
	debug_mesh_builder mb;
	advancing_front af(csg.sphere(), &mb, .1, 100);

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 553905614020596366ull);
	EXPECT_EQ(mb.failed_follows, 0);
	EXPECT_EQ(mb.triangles.size(), 440);
	EXPECT_EQ(check(mb.triangles).total_intersections, 0);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

TEST(advancing_front_no_change, Sphere) {
	csg csg(12345);

	debug_mesh_builder mb;
	advancing_front af(csg.sphere(10), &mb, .5, 100);

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 17961521605756299668ull);
	EXPECT_EQ(mb.failed_follows, 0);
	EXPECT_EQ(mb.triangles.size(), 8222);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

TEST(advancing_front_no_change, Cube) {
	csg csg(12345);

	debug_mesh_builder mb;
	advancing_front af(csg.cube(vec3(20, 20, 20)), &mb, .5, 100);

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 14471321065335532055ull);
	EXPECT_EQ(mb.failed_follows, 0);
	EXPECT_EQ(mb.triangles.size(), 13961);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

TEST(advancing_front_no_change, Composition) {
	debug_mesh_builder mb;
	csg csg(12345);
	advancing_front af(test::create_sphere_tunnel(csg, 20), &mb, 3, 100);

	EXPECT_TRUE(af.try_find_surface({0.1, -0.2, 0.3}));
	// build full sphere
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 942349903305627741ull);
	EXPECT_EQ(mb.failed_follows, 2);
	EXPECT_EQ(mb.triangles.size(), 877);
	EXPECT_EQ(check(mb.triangles).total_intersections, 0);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

TEST(advancing_front_no_change, Noise1) {
	csg csg(12345);

	debug_mesh_builder mb;
	advancing_front af(csg.noise(40), &mb, 7, 50);

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 11202015868412559144ull);
	EXPECT_EQ(mb.failed_follows, 0);
	EXPECT_EQ(mb.triangles.size(), 274);
	EXPECT_EQ(check(mb.triangles).total_intersections, 0);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

TEST(advancing_front_no_change, Noise2) {
	csg csg(12345);

	debug_mesh_builder mb;
	advancing_front af(csg.noise(20), &mb, 7, 50);

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 14998992510276784168ull);
	EXPECT_EQ(mb.failed_follows, 1);
	EXPECT_EQ(mb.triangles.size(), 433);
	EXPECT_EQ(check(mb.triangles).total_intersections, 0);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

TEST(advancing_front_no_change, Noise3) {
	csg csg(12345);

	debug_mesh_builder mb;
	advancing_front af(csg.noise(10), &mb, 5, 30);

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 15026325339014901936ull);
	EXPECT_EQ(mb.failed_follows, 7);
	EXPECT_EQ(mb.triangles.size(), 345);
	EXPECT_EQ(check(mb.triangles).total_intersections, 0);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

TEST(advancing_front_no_change, Noise5) {
	csg csg(12345);

	debug_mesh_builder mb;
	advancing_front af(csg.noise(5), &mb, 3, 25);

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_EQ(mb.hash, 2779804090611705740ull);
	EXPECT_EQ(mb.failed_follows, 36);
	EXPECT_EQ(mb.triangles.size(), 1098);
	EXPECT_EQ(check(mb.triangles).total_intersections, 5);
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
}

inline long long time_scenario(const std::string& name, const volume* volume, double edgeLen, double radius) {
	debug_mesh_builder mb;
	advancing_front af(volume, &mb, edgeLen, radius);
	const timer t;
	EXPECT_TRUE(af.try_find_surface(vec3(0.41, 0.01, -0.23)));
	while (af.step(vec3d::zero, 1)) {
	}
	const auto ms = t.millie_seconds();
	std::cout << name << ": " << ms << " ms\n";
	af.get_surface_memory().delete_removed();
	af.get_surface_memory().validate();
	return ms;
}

#ifndef DEVELOPMENT
TEST(advancing_front_no_change, Performance) {
	csg csg(12345);

	std::stringstream ss;
	long long total = 0;
	total += time_scenario("unit-sphere", csg.sphere(), 0.0025, 50);
	total += time_scenario("unit-cube", csg.cube(), 0.0025, 50);
	total += time_scenario("unit-noise", csg.noise_seed(123), 0.5, 20);

	total += time_scenario("large-sphere", csg.sphere(100), 0.5, 150);
	total += time_scenario("large-box", csg.cube({100, 100, 1}), 0.25, 150);

	total += time_scenario("noise-sphere", csg.add_values({csg.sphere(50), csg.noise_seed(123, 0.6)}), 0.5, 150);
	total += time_scenario("noise-box", csg.add_values({csg.cube(vec3(100, 100, 1)), csg.noise_seed(123)}), 0.5, 150);

	total += time_scenario("noisy-planet", test::create_noisy_planet(csg, 100), 3., 150);
	std::cout << "total: " << total << " ms\n";
	// release: 18092ms 2021-10-01
	// release: 17685ms 2021-10-01  getValue(x,y,z) instead of getValue(const Vec3&)
}
#endif
}
