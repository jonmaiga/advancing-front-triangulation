#include <gtest/gtest.h>

#include "afront/edge.h"
#include "afront/node.h"

namespace playchilla {
TEST(edge, Basic) {
	node a(vec3d::zero, vec3d::Y);
	node b(vec3(1, 0, 0), vec3d::Y);
	node c(vec3(1, 0, 1), vec3d::Y);
	node d(vec3(1, 1, 1), vec3d::Y);
	edge ab(&a, &b);
	edge ac(&a, &c);
	edge bc(&b, &c);
	edge cd(&c, &d);

	// has node
	EXPECT_TRUE(ab.has_node(&a));
	EXPECT_TRUE(ab.has_node(&b));
	EXPECT_TRUE(!ab.has_node(&c));

	// other
	EXPECT_TRUE(ab.get_other(&a) == &b);
	EXPECT_TRUE(ab.get_other(&b) == &a);

	// equals
	//EXPECT_TRUE(ab.equals(&ab));
	//EXPECT_TRUE(!ab.equals(&a));
	//EXPECT_TRUE(!ab.equals(&ac));

	// common node
	EXPECT_TRUE(ab.has_common_node(&ac));
	EXPECT_TRUE(ab.get_common_node(&ac) == &a);
	EXPECT_TRUE(ac.get_common_node(&bc) == &c);
	EXPECT_TRUE(ab.get_common_node(&cd) == nullptr);
}
}
