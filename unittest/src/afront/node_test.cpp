#include <gtest/gtest.h>

#include "afront/node.h"
#include "afront/edge.h"

namespace playchilla {

TEST(node, Empty) {
    node a(vec3d::zero, vec3d::Y);
    node b(vec3(1, 0, 0), vec3d::Y);
    EXPECT_EQ(a.get_edge_to(&b), nullptr);
    EXPECT_FALSE(a.has_edge_to(&b));
    EXPECT_FALSE(a.is_removed());
    EXPECT_TRUE(a.pos == vec3d::zero);
    EXPECT_TRUE(a.normal == vec3d::Y);
    EXPECT_TRUE(a.edges.empty());
    a.mark_for_removal();
    EXPECT_TRUE(a.is_removed());
}

TEST(node, Basic) {
    node a(vec3d::zero, vec3d::Y);
    node b(vec3(1, 0, 0), vec3d::Y);
    node c(vec3(1, 0, 1), vec3d::Y);
    // get_edge_to
    EXPECT_TRUE(!a.has_edge_to(&b));
    EXPECT_TRUE(a.get_edge_to(&b) == nullptr);

    edge ab(&a, &b);
    a.edges.push_back(&ab);
    b.edges.push_back(&ab);
    EXPECT_TRUE(a.has_edge_to(&b));
    EXPECT_TRUE(!a.has_edge_to(&c));
    EXPECT_TRUE(a.get_edge_to(&b) == &ab);
    EXPECT_TRUE(b.has_edge_to(&a));
    EXPECT_TRUE(!b.has_edge_to(&c));
    EXPECT_TRUE(b.get_edge_to(&a) == &ab);

    edge ac(&a, &c);
    a.edges.push_back(&ac);
    c.edges.push_back(&ac);
    EXPECT_TRUE(a.has_edge_to(&b));
    EXPECT_TRUE(a.has_edge_to(&c));
    EXPECT_TRUE(a.get_edge_to(&c) == &ac);
}

}
