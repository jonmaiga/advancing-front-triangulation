#include <gtest/gtest.h>

#include "afront/surface_memory.h"
#include "afront/edge.h"

namespace playchilla {

TEST(surface_memory, Empty) {
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    EXPECT_TRUE(sm.get_front().empty());
    EXPECT_EQ(sm.get_node_cells(), 0);
    EXPECT_EQ(sm.get_node_count(), 0);
    EXPECT_EQ(sm.pop_edge(), nullptr);
    EXPECT_TRUE(sm.get_nodes(vec3d::zero,200).empty());
}

TEST(surface_memory, Basic) {
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    auto* a = sm.add_node(vec3d::zero, vec3d::Y);
    EXPECT_TRUE(sm.get_nodes(vec3d::zero, 1)[0] == a);
    EXPECT_TRUE(sm.get_nodes(vec3(1.1,0,0), 1).empty());
    EXPECT_EQ(sm.get_node_count(), 1);
    EXPECT_EQ(sm.get_node_cells(), 1);
    EXPECT_EQ(sm.pop_edge(), nullptr);

    auto* b = sm.add_node(vec3(2,0,0), vec3d::Y);
    EXPECT_TRUE(sm.get_nodes(vec3d::zero, 1)[0] == a);
    EXPECT_TRUE(sm.get_nodes(vec3(1.1,0,0), 1)[0] == b);
    EXPECT_EQ(sm.get_node_count(), 2);
    EXPECT_EQ(sm.get_node_cells(), 1);
    EXPECT_EQ(sm.pop_edge(), nullptr);

    const edge* e = sm.push(a, b);
    EXPECT_EQ(e->get_other(a), b);
    EXPECT_EQ(e->get_other(b), a);
    EXPECT_EQ(sm.get_front().front(), e);
}

TEST(surface_memory, RemoveEdges) {
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    auto* a = sm.add_node(vec3d::zero, vec3d::Y);
    EXPECT_EQ(sm.get_node_count(), 1);
    EXPECT_EQ(sm.get_edge_count(), 0);

    auto* b = sm.add_node(vec3(2,0,0), vec3d::Y);
    EXPECT_EQ(sm.get_node_count(), 2);
    EXPECT_EQ(sm.get_edge_count(), 0);

    const edge* e = sm.push(a, b);
    EXPECT_EQ(sm.get_node_count(), 2);
    EXPECT_EQ(sm.get_edge_count(), 1);

    sm.collapse_node(a);
    sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 0);
    EXPECT_EQ(sm.get_edge_count(), 0);
}

TEST(surface_memory, RemoveOneNode) {
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    node* a = sm.add_node(vec3d::zero, vec3d::Y);
    sm.collapse_node(a);
    EXPECT_TRUE(sm.get_nodes(vec3d::zero, 10).empty());
    EXPECT_TRUE(sm.pop_edge() == nullptr);
    EXPECT_EQ(nh.get_cell_count(), 0);
    EXPECT_TRUE(sm.pop_edge() == nullptr);
    sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 0);
    EXPECT_EQ(sm.get_edge_count(), 0);
}

TEST(surface_memory, RemoveTwoNodes) {
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    node* a = sm.add_node(vec3d::zero, vec3d::Y);
    node* b = sm.add_node(vec3(1, 0, 0), vec3d::Y);

    edge* ab = sm.push(a, b);
    EXPECT_TRUE(a->has_edge_to(b));
    EXPECT_TRUE(a->get_edge_to(b) == ab);

    sm.collapse_node(a);
    EXPECT_TRUE(!a->has_edge_to(b));
    EXPECT_TRUE(!b->has_edge_to(a));
    EXPECT_TRUE(a->edges.empty());
    EXPECT_TRUE(b->edges.empty());
    EXPECT_TRUE(a->is_removed());
    EXPECT_TRUE(b->is_removed());
    EXPECT_TRUE(ab->is_used());
    EXPECT_TRUE(ab->has_node(a));
    EXPECT_TRUE(ab->has_node(b));
    EXPECT_EQ(nh.get_cell_count(), 0);

    EXPECT_TRUE(sm.pop_edge() == ab);
    EXPECT_TRUE(sm.pop_edge() == nullptr);

    sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 0);
    EXPECT_EQ(sm.get_edge_count(), 0);
}

TEST(surface_memory, RemoveThreeNodesTwoEdges) {
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    node* a = sm.add_node(vec3d::zero, vec3d::Y);
    node* b = sm.add_node(vec3(1, 0, 0), vec3d::Y);
    node* c = sm.add_node(vec3(1, 0, 1), vec3d::Y);
    edge* ab = sm.push(a, b);
    edge* ac = sm.push(a, c);

    sm.collapse_node(b);
    EXPECT_TRUE(!a->has_edge_to(b));
    EXPECT_TRUE(!b->has_edge_to(a));
    EXPECT_TRUE(a->has_edge_to(c));
    EXPECT_TRUE(c->has_edge_to(a));
    EXPECT_TRUE(b->edges.empty());
    EXPECT_TRUE(nh.get_value_count() == 2);
    EXPECT_TRUE(sm.get_nodes(a->pos, 0.1)[0] == a);
    EXPECT_TRUE(sm.get_nodes(b->pos, 0.1).empty());
    EXPECT_TRUE(sm.get_nodes(c->pos, 0.1)[0] == c);
    EXPECT_TRUE(ab->is_used());
    EXPECT_TRUE(!ac->is_used());
    EXPECT_TRUE(!a->is_removed());
    EXPECT_TRUE(b->is_removed());
    EXPECT_TRUE(!c->is_removed());

    EXPECT_TRUE(sm.pop_edge() == ab);
    EXPECT_TRUE(sm.pop_edge() == ac);
    EXPECT_TRUE(sm.pop_edge() == nullptr);

    sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 2);
    EXPECT_EQ(sm.get_edge_count(), 1);
}

TEST(surface_memory, RemoveThreeNodesTwoEdgesJoint) {
    // Scenario 4
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    node*a = sm.add_node(vec3d::zero, vec3d::Y);
    node*b = sm.add_node(vec3(1, 0, 0), vec3d::Y);
    node*c = sm.add_node(vec3(1, 0, 1), vec3d::Y);
    edge* ab = sm.push(a, b);
    edge* ac = sm.push(a, c);

    sm.collapse_node(a);
    EXPECT_TRUE(!a->has_edge_to(b));
    EXPECT_TRUE(!b->has_edge_to(a));
    EXPECT_TRUE(!a->has_edge_to(c));
    EXPECT_TRUE(!c->has_edge_to(a));
    EXPECT_TRUE(a->edges.empty());
    EXPECT_TRUE(b->edges.empty());
    EXPECT_TRUE(c->edges.empty());
    EXPECT_TRUE(a->is_removed());
    EXPECT_TRUE(b->is_removed());
    EXPECT_TRUE(c->is_removed());
    EXPECT_TRUE(ab->is_used());
    EXPECT_TRUE(ac->is_used());
    EXPECT_EQ(nh.get_cell_count(), 0);
    EXPECT_TRUE(sm.pop_edge() == ab);
    EXPECT_TRUE(sm.pop_edge() == ac);
    EXPECT_TRUE(sm.pop_edge() == nullptr);

	sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 0);
    EXPECT_EQ(sm.get_edge_count(), 0);
}

TEST(surface_memory, RemoveTriangle) {
    // Scenario 5
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    node* a = sm.add_node(vec3d::zero, vec3d::Y);
    node* b = sm.add_node(vec3(1, 0, 0), vec3d::Y);
    node* c = sm.add_node(vec3(1, 0, 1), vec3d::Y);
    edge* ab = sm.push(a, b);
    edge* ac = sm.push(a, c);
    edge* bc = sm.push(b, c);

    sm.collapse_node(a);
    EXPECT_TRUE(!a->has_edge_to(b));
    EXPECT_TRUE(!b->has_edge_to(a));
    EXPECT_TRUE(!a->has_edge_to(c));
    EXPECT_TRUE(!c->has_edge_to(a));
    EXPECT_TRUE(b->has_edge_to(c));
    EXPECT_TRUE(c->has_edge_to(b));

    EXPECT_TRUE(a->edges.empty());
    EXPECT_TRUE(b->edges.size() == 1);
    EXPECT_TRUE(c->edges.size() == 1);

    EXPECT_TRUE(a->is_removed());
    EXPECT_TRUE(!b->is_removed());
    EXPECT_TRUE(!c->is_removed());
    EXPECT_TRUE(ab->is_used());
    EXPECT_TRUE(ac->is_used());
    EXPECT_TRUE(!bc->is_used());
    EXPECT_TRUE(nh.get_value_count() == 2);
    EXPECT_TRUE(sm.get_nodes(a->get_pos(), 0.1).empty());
    EXPECT_TRUE(sm.get_nodes(b->get_pos(), 0.1)[0] == b);
    EXPECT_TRUE(sm.get_nodes(c->get_pos(), 0.1)[0] == c);
    EXPECT_TRUE(sm.pop_edge() == ab);
    EXPECT_TRUE(sm.pop_edge() == ac);
    EXPECT_TRUE(sm.pop_edge() == bc);
    EXPECT_TRUE(sm.pop_edge() == nullptr);

	sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 2);
    EXPECT_EQ(sm.get_edge_count(), 1);
}

TEST(surface_memory, RemoveThreeNodesTwoEdgesJointExtend) {
    // Scenario 6
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    node* a = sm.add_node(vec3d::zero, vec3d::Y);
    node* b = sm.add_node(vec3(1, 0, 0), vec3d::Y);
    node* c = sm.add_node(vec3(1, 0, 1), vec3d::Y);
    node* d = sm.add_node(vec3(1, 1, 1), vec3d::Y);
    edge* ab = sm.push(a, b);
    edge* ac = sm.push(a, c);
    edge* cd = sm.push(c, d);

    sm.collapse_node(a);
    EXPECT_TRUE(!a->has_edge_to(b));
    EXPECT_TRUE(!b->has_edge_to(a));
    EXPECT_TRUE(!a->has_edge_to(c));
    EXPECT_TRUE(!c->has_edge_to(a));
    EXPECT_TRUE(a->edges.empty());
    EXPECT_TRUE(b->edges.empty());
    EXPECT_TRUE(c->edges.size() == 1);
    EXPECT_TRUE(a->is_removed());
    EXPECT_TRUE(b->is_removed());
    EXPECT_TRUE(!c->is_removed());
    EXPECT_TRUE(!d->is_removed());
    EXPECT_TRUE(ab->is_used());
    EXPECT_TRUE(ac->is_used());
    EXPECT_TRUE(!cd->is_used());
    EXPECT_TRUE(nh.get_value_count() == 2);
    EXPECT_TRUE(sm.get_nodes(b->get_pos(), 0.1).empty());
    EXPECT_TRUE(sm.get_nodes(c->get_pos(), 0.1)[0] == c);
    EXPECT_TRUE(sm.get_nodes(d->get_pos(), 0.1)[0] == d);

    EXPECT_TRUE(sm.pop_edge() == ab);
    EXPECT_TRUE(sm.pop_edge() == ac);
    EXPECT_TRUE(sm.pop_edge() == cd);
    EXPECT_TRUE(sm.pop_edge() == nullptr);

	sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 2);
    EXPECT_EQ(sm.get_edge_count(), 1);
}

TEST(surface_memory, RemoveDragon) {
    // Scenario 7
    surface_memory sm(100);
    const auto& nh = sm.get_node_hash();
    node* a = sm.add_node(vec3d::zero, vec3d::Y);
    node* b = sm.add_node(vec3(1, 0, 0), vec3d::Y);
    node* c = sm.add_node(vec3(1, 0, 1), vec3d::Y);
    node* d = sm.add_node(vec3(1, 1, 1), vec3d::Y);
    edge* ab = sm.push(a, b);
    edge* ac = sm.push(a, c);
    edge* ad = sm.push(a, d);
    edge* cd = sm.push(c, d);
    edge* bd = sm.push(b, d);

    sm.collapse_node(a);
    EXPECT_TRUE(!a->has_edge_to(b));
    EXPECT_TRUE(!b->has_edge_to(a));
    EXPECT_TRUE(!a->has_edge_to(c));
    EXPECT_TRUE(!c->has_edge_to(a));
    EXPECT_TRUE(!a->has_edge_to(d));
    EXPECT_TRUE(!d->has_edge_to(a));
    EXPECT_TRUE(a->edges.empty());
    EXPECT_TRUE(b->edges.size() == 1);
    EXPECT_TRUE(c->edges.size() == 1);
    EXPECT_TRUE(d->edges.size() == 2);
    EXPECT_TRUE(a->is_removed());
    EXPECT_TRUE(!b->is_removed());
    EXPECT_TRUE(!c->is_removed());
    EXPECT_TRUE(!d->is_removed());
    EXPECT_TRUE(ab->is_used());
    EXPECT_TRUE(ac->is_used());
    EXPECT_TRUE(ad->is_used());
    EXPECT_TRUE(!cd->is_used());
    EXPECT_TRUE(!bd->is_used());
    EXPECT_TRUE(nh.get_value_count() == 3);
    EXPECT_TRUE(sm.get_nodes(b->get_pos(), 0.1)[0] == b);
    EXPECT_TRUE(sm.get_nodes(c->get_pos(), 0.1)[0] == c);
    EXPECT_TRUE(sm.get_nodes(d->get_pos(), 0.1)[0] == d);

    EXPECT_TRUE(sm.pop_edge() == ab);
    EXPECT_TRUE(sm.pop_edge() == ac);
    EXPECT_TRUE(sm.pop_edge() == ad);
    EXPECT_TRUE(sm.pop_edge() == cd);
    EXPECT_TRUE(sm.pop_edge() == bd);
    EXPECT_TRUE(sm.pop_edge() == nullptr);

	sm.delete_removed();
    EXPECT_EQ(sm.get_node_count(), 3);
    EXPECT_EQ(sm.get_edge_count(), 2);
}

}