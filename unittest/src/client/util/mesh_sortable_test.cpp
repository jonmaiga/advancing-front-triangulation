#include <gtest/gtest.h>

#include "client/render/mesh_sortable.h"

namespace playchilla {
TEST(mesh_sortable, BitStrideTest) {
	{
		bit_stride bs("a", 0, 1);
		EXPECT_EQ(bs.start, 0);
		EXPECT_EQ(bs.bits, 1);
		EXPECT_EQ(bs.max, 1);
		EXPECT_EQ(bs.mask, 1);
		EXPECT_EQ(bs.stop, 1);
		EXPECT_EQ(bs.set(0, 1), 1);
		EXPECT_EQ(bs.set(0, 0), 0);
		EXPECT_EQ(bs.set(1, 0), 0);
		EXPECT_EQ(bs.set(1, 1), 1);
		EXPECT_EQ(bs.set(2, 1), 3);

		bit_stride next = bs.next("b", 1);
		EXPECT_EQ(next.start, 1);
		EXPECT_EQ(next.bits, 1);
		EXPECT_EQ(next.max, 1);
		EXPECT_EQ(next.mask, 2);
		EXPECT_EQ(next.stop, 2);
		EXPECT_EQ(next.set(0, 1), 2);
		EXPECT_EQ(next.set(0, 0), 0);
		EXPECT_EQ(next.set(2, 0), 0);
		EXPECT_EQ(next.set(2, 1), 2);
		EXPECT_EQ(next.set(1, 1), 3);
	}

	{
		bit_stride bs("a", 1, 1);
		EXPECT_EQ(bs.set(0, 1), 2);
		EXPECT_EQ(bs.set(0, 0), 0);
		EXPECT_EQ(bs.set(1, 0), 1);
		EXPECT_EQ(bs.set(1, 1), 3);
		EXPECT_EQ(bs.set(2, 1), 2);
	}

	{
		bit_stride bs("a", 2, 2);
		EXPECT_EQ(bs.set(0, 1), 4);
		EXPECT_EQ(bs.set(3, 1), 7);
		EXPECT_EQ(bs.set(3, 0), 3);
		EXPECT_EQ(bs.set(0, 1), 4);
		EXPECT_EQ(bs.set(1 + 2 + 4 + 8 + 16 + 32, 0), 1 + 2 + 16 + 32);
		EXPECT_EQ(bs.set(1 + 2 + 4 + 8 + 16 + 32, 1), 1 + 2 + 4 + 16 + 32);
		EXPECT_EQ(bs.set(1 + 2 + 4 + 8 + 16 + 32, 2), 1 + 2 + 8 + 16 + 32);
		EXPECT_EQ(bs.set(1 + 2 + 4 + 8 + 16 + 32, 3), 1 + 2 + 4 + 8 + 16 + 32);
	}
}

TEST(mesh_sortable, Basic) {
	mesh_sortable s1 = mesh_sortable().layer(0).blend(true);
	mesh_sortable s2 = mesh_sortable().layer(1).blend(true);
	EXPECT_LT(s1.get_data(), s2.get_data());

	s1.blend(false);
	EXPECT_LT(s1.get_data(), s2.get_data());

	s1.blend(true).layer(1);
	EXPECT_EQ(s1.get_data(), s2.get_data());

	mesh_sortable s3 = mesh_sortable().layer(1).blend(true);
	mesh_sortable s4 = mesh_sortable().layer(1).blend(true);
	EXPECT_EQ(s3.get_data(), s4.get_data());
}

TEST(mesh_sortable, OpaqueFirst) {
	// opaque first front->back
	// blend then back->front
	mesh_sortable b1 = mesh_sortable().layer(0).blend(true).depth_back_front(100);
	mesh_sortable o1 = mesh_sortable().layer(0).blend(false).depth_front_back(200);
	mesh_sortable b2 = mesh_sortable().layer(0).blend(true).depth_back_front(200);
	mesh_sortable o2 = mesh_sortable().layer(0).blend(false).depth_front_back(100);
	EXPECT_LT(o1.get_data(), b1.get_data()); // opaque first
	EXPECT_LT(o1.get_data(), b2.get_data()); // opaque first
	EXPECT_LT(o2.get_data(), o1.get_data()); // opaque front to back
	EXPECT_LT(b2.get_data(), b1.get_data()); // blend back to front

	mesh_sortable s3 = mesh_sortable().layer(1).blend(true);
	mesh_sortable s4 = mesh_sortable().layer(1).blend(false);
	EXPECT_LT(s4.get_data(), s3.get_data());
	//assertTrue(b2.getData() < s4.getData());
}
}
