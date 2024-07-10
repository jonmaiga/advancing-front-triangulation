#include <gtest/gtest.h>

#include "afront/advancing_front.h"
#include "client/volume/csg.h"


namespace playchilla {
TEST(advancing_front, SphereTest) {
	csg csg(12345);
	advancing_front af(csg.sphere(10), nullptr, 0.5, 100);
	auto& sm = af.get_surface_memory();
	const auto& nh = sm.get_node_hash();

	EXPECT_TRUE(af.try_find_surface(vec3(1, 0, 0)));
	EXPECT_EQ(sm.get_node_count(), 2);
	auto nodes = nh.get_values(vec3d::zero, 20);

	// create one triangle
	af.step(vec3d::zero, 1);
	EXPECT_EQ(sm.get_node_count(), 3);

	nodes = nh.get_values(vec3d::zero, 20);
	EXPECT_EQ(nodes.size(), 3);

	sm.collapse_node(nodes[1]);
	sm.delete_removed();
	EXPECT_EQ(sm.get_node_count(), 2);


	// recreate it
	af.step(vec3d::zero, 1);
	EXPECT_EQ(nodes.size(), 3);

	// create second triangle
	af.step(vec3d::zero, 1);
	EXPECT_EQ(sm.get_node_count(), 4);
	nodes = nh.get_values(vec3d::zero, 20);
	EXPECT_EQ(nodes.size(), 4);

	// remove joint node (will remove two triangles)
	sm.collapse_node(nodes[2]);
	EXPECT_EQ(nh.get_values(vec3d::zero, 20).size(), 3);

	// recreate it
	af.step(vec3d::zero, 1);
	EXPECT_EQ(nh.get_values(vec3d::zero, 20).size(), 4);

	// build full sphere
	while (af.step(vec3d::zero, 1)) {
	}
	EXPECT_GT(sm.get_node_count(), 1000);
	EXPECT_EQ(nh.get_values(vec3d::zero, 9.9).size(), 0);
	EXPECT_GT(nh.get_values(vec3d::zero, 10.1).size(), 1000);
}
}
