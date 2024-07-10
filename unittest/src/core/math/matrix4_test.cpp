#include <gtest/gtest.h>

#include "core/math/matrix4.h"
#include "core/math/quat_util.h"

namespace playchilla {
TEST(matrix4, Identity) {
    matrix4 m;
    EXPECT_TRUE(m.m00 == 1);
    EXPECT_TRUE(m.m11 == 1);
    EXPECT_TRUE(m.m22 == 1);
    EXPECT_TRUE(m.m33 == 1);
    EXPECT_TRUE(m.get_determinant() == 1);
    EXPECT_TRUE(m.get_scale().equals(1, 1, 1));
    EXPECT_TRUE(m.get_rotation().get_degrees() == 0);
    EXPECT_TRUE(m.get_translation().equals(0, 0, 0));
}

TEST(matrix4, GetPosScaleRot) {
    matrix4 m;

    m.set_translation(1, 2, 3);
    EXPECT_TRUE(m.get_translation().is_near(1, 2, 3));
    EXPECT_TRUE(m.get_scale().is_near(1, 1, 1));
    EXPECT_TRUE(m.get_rotation().get_degrees() == 0);

    m.set_scale(4, 5, 6);
    EXPECT_TRUE(m.get_translation().is_near(0, 0, 0));
    EXPECT_TRUE(m.get_scale().is_near(4, 5, 6));
    EXPECT_TRUE(m.get_rotation().get_degrees() == 0);

    quat r;
    m.set_rotation(r.x, r.y, r.z, r.w);
    EXPECT_TRUE(m.get_translation().is_near(0, 0, 0));
    EXPECT_TRUE(m.get_scale().is_near(1, 1, 1));
    EXPECT_TRUE(m.get_rotation().get_degrees() == 0);

    r = set_from_axis_deg(vec3d::X, 35);
    m.set_rotation(r.x, r.y, r.z, r.w);
    EXPECT_TRUE(m.get_translation().is_near(0, 0, 0));
    EXPECT_TRUE(m.get_scale().is_near(1, 1, 1));
    EXPECT_TRUE(is_near(m.get_rotation().get_degrees(), 35));

    r = set_from_axis_deg(vec3d::Y, -55);
    m.set_rotation(r.x, r.y, r.z, r.w);
    EXPECT_TRUE(m.get_translation().is_near(0, 0, 0));
    EXPECT_TRUE(m.get_scale().is_near(1, 1, 1));
    EXPECT_TRUE(is_near(m.get_rotation().get_degrees(), 55));

    r = set_from_axis_deg(vec3d::Z, 130);
    m.set_rotation(r.x, r.y, r.z, r.w);
    EXPECT_TRUE(m.get_translation().is_near(0, 0, 0));
    EXPECT_TRUE(m.get_scale().is_near(1, 1, 1));
    EXPECT_TRUE(is_near(m.get_rotation().get_degrees(), 130));

    m.set(1, 2, 3, r.x, r.y, r.z, r.w, 1, 1, 1);
    quat rot = m.get_rotation();
    EXPECT_TRUE(m.get_translation().is_near(1, 2, 3));
    EXPECT_TRUE(is_near(rot.get_degrees(), 130));
}

TEST(matrix4, LargeValues) {
    matrix4 p;
    p.set_translation(0, 10000000, 0);

    matrix4 c;
    c.set_translation(0, 1, 0);
    matrix4 o = p.mul(c);
    EXPECT_TRUE(o.get_translation().equals(0, 10000000 + 1, 0));

    c.set_translation(0, 0.5, 0);
    o = p.mul(c);
    vec3 t = o.get_translation();
    EXPECT_TRUE(t.equals(0, 10000000 + 0.5, 0));

    c.set_translation(0.3, 0.25, -2.2);
    o = p.mul(c);
    t = o.get_translation();
    EXPECT_TRUE(t.equals(0.3, 10000000 + 0.25, -2.2));
}

}
