#include <gtest/gtest.h>

#include "core/math/quat_util.h"
#include "core/math/vec3.h"

namespace playchilla {
TEST(quat, Identity) {
	quat q;
	EXPECT_TRUE(q.x == 0);
	EXPECT_TRUE(q.y == 0);
	EXPECT_TRUE(q.z == 0);
	EXPECT_TRUE(q.w == 1);
	EXPECT_TRUE(q.is_normalized());
	EXPECT_TRUE(q.length() == 1);
	EXPECT_TRUE(q.length_sqr() == 1);
	EXPECT_TRUE(q.get_degrees() == 0);
	EXPECT_TRUE(q.get_radians() == 0);
}


TEST(quat, BasicRotationX) {
	quat q = set_from_axis_deg(vec3d::X, 90);

	vec3 p(1, 0, 0);
	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(1, 0, 0));

	p = rotate_vec3({0, 1, 0}, q);
	EXPECT_TRUE(p.is_near(0, 0, 1));

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, -1, 0));
}


TEST(quat, BasicRotationY) {
	quat q = set_from_axis_deg(vec3d::Y, 90);
	vec3 p(0, 1, 0);

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, 1, 0));

	p = rotate_vec3({1, 0, 0}, q);
	EXPECT_TRUE(p.is_near(0, 0, -1));

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(-1, 0, 0));
}


TEST(quat, BasicRotationZ) {
	quat q = set_from_axis_deg(vec3d::Z, 90);
	vec3 p(0, 0, 1);

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, 0, 1));

	p = rotate_vec3({0, 1, 0}, q);
	EXPECT_TRUE(p.is_near(-1, 0, 0));

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, -1, 0));
}


TEST(quat, BasicRotationYtoX) {
	quat q = set_from_two_vectors(vec3d::Y, vec3d::X);

	vec3 p(0, 0, 1);
	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, 0, 1));

	p = rotate_vec3({0, 1, 0}, q);
	EXPECT_TRUE(p.is_near(1, 0, 0));

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, -1, 0));
}


TEST(quat, BasicRotationXtoZ) {
	quat q = set_from_two_vectors(vec3d::X, vec3d::Z);

	vec3 p(0, 1, 0);
	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, 1, 0));

	p = rotate_vec3({1, 0, 0}, q);
	EXPECT_TRUE(p.is_near(0, 0, 1));

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(-1, 0, 0));
}


TEST(quat, CompositeRotation) {
	quat y_tox = set_from_two_vectors(vec3d::Y, vec3d::X);
	quat x_toz = set_from_two_vectors(vec3d::X, vec3d::Z);
	quat both = x_toz.mul(y_tox);
	EXPECT_TRUE(both.is_normalized());
	vec3 p(0, 1, 0);
	p = rotate_vec3(p, both);
	EXPECT_TRUE(p.is_near(0, 0, 1));
}


TEST(quat, Conjugate) {
	quat q = set_from_axis_deg(vec3d::X, 90);
	q = q.conjugate();

	quat qi = set_from_axis_deg(vec3d::X, -90);
	EXPECT_TRUE(qi.is_exactly(q));

	vec3 p(1, 0, 0);
	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(1, 0, 0));

	p = rotate_vec3({0, 1, 0}, q);
	EXPECT_TRUE(p.is_near(0, 0, -1));

	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(0, -1, 0));
}


TEST(quat, RotateWithOffset) {
	quat q = set_from_axis_deg(vec3d::X, 90);
	q = q.conjugate();

	vec3 p(1, 1, 0);
	p = rotate_vec3(p, q);
	EXPECT_TRUE(p.is_near(1, 0, -1));
}


TEST(quat, PlatformExample) {
	quat p = set_from_axis_deg(vec3d::X, 45);
	quat t;
	vec3 wanted_up = rotate_vec3(vec3d::Y, p);
	vec3 current_up = rotate_vec3(vec3d::Y, t);
	quat diff = set_from_two_vectors(current_up, wanted_up);
	vec3 transformed = rotate_vec3(current_up, diff);
	EXPECT_TRUE(transformed.is_near(wanted_up));
}


TEST(quat, RotationAround) {
	quat x = set_from_axis_deg(vec3d::X, 35);
	quat y = set_from_axis_deg(vec3d::Y, 55);
	quat z = set_from_axis_deg(vec3d::Z, 75);

	EXPECT_TRUE(is_near(x.get_around_x_degrees(), 35));
	EXPECT_TRUE(is_near(x.get_around_y_degrees(), 0));
	EXPECT_TRUE(is_near(x.get_around_z_degrees(), 0));

	EXPECT_TRUE(is_near(y.get_around_x_degrees(), 0));
	EXPECT_TRUE(is_near(y.get_around_y_degrees(), 55));
	EXPECT_TRUE(is_near(y.get_around_z_degrees(), 0));

	EXPECT_TRUE(is_near(z.get_around_x_degrees(), 0));
	EXPECT_TRUE(is_near(z.get_around_y_degrees(), 0));
	EXPECT_TRUE(is_near(z.get_around_z_degrees(), 75));
}
}
