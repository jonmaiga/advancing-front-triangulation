#include <gtest/gtest.h>

#include "core/math/transform.h"

namespace playchilla {
//////////////////////////////////////////////////
// Basic translate, scale and rotate
//////////////////////////////////////////////////

TEST(transform, BasicTranslate) {
	transform t;
	EXPECT_EQ(t.get_pos(), vec3d::zero);
	EXPECT_EQ(t.set_pos(vec3(1, 1, 1)).get_pos(), vec3(1, 1, 1));
	EXPECT_EQ(t.set_pos(vec3(2, 3, 4)).get_pos(), vec3(2, 3, 4));
	EXPECT_EQ(t.add_pos(-1, -2, -3).get_pos(), vec3(1, 1, 1));
	EXPECT_EQ(t.add_pos(vec3(-1, -2, -3)).get_pos(), vec3(0, -1, -2));
	EXPECT_EQ(t.get_matrix().get_translation(), vec3(0,-1,-2));
}

TEST(transform, BasicScale) {
	transform t;
	EXPECT_EQ(t.get_scale(), vec3(1, 1, 1));
	EXPECT_EQ(t.set_scale(2, 3, 4).get_scale(), vec3(2, 3, 4));
	EXPECT_TRUE(t.set_scale(4).get_scale().equals(4, 4, 4));
	EXPECT_EQ(t.get_matrix().get_scale(), vec3(4,4,4));
}

TEST(transform, BasicRotation) {
	transform t;
	EXPECT_EQ(t.get_rotation().get_degrees(), 0);
	EXPECT_EQ(t.set_rotate(set_from_axis_deg(vec3d::X, 90)).get_rotation().get_degrees(), 90);
	EXPECT_NEAR(t.rotate(set_from_axis_deg(vec3d::X, 90)).get_rotation().get_degrees(), 180, 1e-6);
	EXPECT_NEAR(t.get_matrix().get_rotation().get_degrees(), 180, 1e-6);
}

TEST(transform, BasicPosRotation) {
	transform t;
	t.set_pos(0, 0, 1).set_rotate(set_from_axis_deg(vec3d::X, 90));
	EXPECT_EQ(t.get_rotation().get_degrees(), 90);
	EXPECT_TRUE(t.get_pos().is_near(0, 0, 1));
	EXPECT_TRUE(t.get_matrix().get_translation().is_near(0,0,1));
	EXPECT_NEAR(t.get_matrix().get_rotation().get_degrees(), 90, 1e-6);
}


TEST(transform, BasicCopy) {
	transform t2;
	t2.set_pos(vec3(1, 2, 3));
	t2.set_scale(vec3(3, 2, 1));
	t2.set_rotate_y(90);
	transform t(t2);
	EXPECT_EQ(t.get_pos(), vec3(1, 2, 3));
	EXPECT_EQ(t.get_scale(), vec3(3, 2, 1));
	EXPECT_EQ(t.get_rotation().get_degrees(), 90);
}


/*
//////////////////////////////////////////////////
// Translate, scale and rotate with parent
//////////////////////////////////////////////////

TEST(transform, TranslateParent) {
	transform p;
	p.set_pos(5, 6, 7);

	transform t;
	p.add_child(&t);

	EXPECT_EQ(t.get_pos(), vec3d::Zero);
	EXPECT_TRUE(t.get_global_pos().equals(5, 6, 7));

	t.set_global_pos(vec3(1, 1, 1));
	EXPECT_TRUE(t.get_pos().equals(-4, -5, -6));
	EXPECT_TRUE(t.get_global_pos().equals(1, 1, 1));

	t.set_pos(vec3(2, 3, 4));
	EXPECT_TRUE(t.get_pos().equals(2, 3, 4));
	EXPECT_TRUE(t.get_global_pos().equals(7, 9, 11));

	t.add_pos(-1, -2, -3);
	EXPECT_TRUE(t.get_pos().equals(1, 1, 1));
	EXPECT_TRUE(t.get_global_pos().equals(6, 7, 8));

	t.set_global_pos(vec3(-4, -6, -8));
	EXPECT_TRUE(t.get_pos().equals(-9, -12, -15));
	EXPECT_TRUE(t.get_global_pos().equals(-4, -6, -8));

	p.add_pos(-5, -6, -7);
	EXPECT_TRUE(t.get_pos().equals(-9, -12, -15));
	EXPECT_TRUE(t.get_global_pos().equals(-9, -12, -15));
}


TEST(transform, ScaleParent) {
	transform p;
	p.set_scale(1, 2, 3);

	transform t;
	p.add_child(&t);
	EXPECT_TRUE(t.get_scale().equals(1, 1, 1));
	EXPECT_TRUE(t.get_global_scale().equals(1, 2, 3));

	t.set_scale(2, 3, 4);
	EXPECT_TRUE(t.get_scale().equals(2, 3, 4));
	EXPECT_TRUE(t.get_global_scale().equals(2, 6, 12));

	t.set_scale(4);
	EXPECT_TRUE(t.get_scale().equals(4, 4, 4));
	EXPECT_TRUE(t.get_global_scale().equals(4, 8, 12));
}


TEST(transform, RotationParent) {
	transform p;
	p.set_rotate_x(90);

	transform t;
	p.add_child(&t);
	EXPECT_TRUE(t.get_rotation().get_degrees() == 0);
	EXPECT_TRUE(is_near(t.get_global_rotation().get_degrees(), 90, 0.00001));

	t.set_rotate(set_from_axis_deg(vec3d::X, 90));
	EXPECT_TRUE(t.get_rotation().get_degrees() == 90);
	EXPECT_TRUE(is_near(t.get_global_rotation().get_degrees(), 180, 0.00001));

	t.rotate(set_from_axis_deg(vec3d::X, 80));
	EXPECT_TRUE(is_near(t.get_rotation().get_degrees(), 170, 0.00001));

	double d = t.get_global_rotation().get_degrees();
	EXPECT_TRUE(is_near(t.get_global_rotation().get_degrees(), 260, 0.0001));
}


TEST(transform, GlobalRotateParent) {
	transform p;
	p.set_rotate_x(45);

	transform t;
	p.add_child(&t);

	t.set_global_rotation(set_from_axis_deg(vec3d::X, 45));
	EXPECT_TRUE(t.get_rotation().get_degrees() == 0);
	EXPECT_TRUE(is_near(t.get_global_rotation().get_degrees(), 45, 0.0001));

	t.set_global_rotation(set_from_axis_deg(vec3d::X, 55));
	double d = t.get_rotation().get_degrees();
	EXPECT_TRUE(is_near(t.get_rotation().get_degrees(), 10, 0.0001));
	d = t.get_global_rotation().get_degrees();
	EXPECT_TRUE(is_near(t.get_global_rotation().get_degrees(), 55, 0.0001));
}


TEST(transform, GlobalRotateXZParent) {
	transform p;
	p.set_rotate(45, 0, 45);
	vec3 u = p.get_global_up();
	vec3 f = p.get_global_forward();

	transform t;
	p.add_child(&t);

	t.set_global_rotation(set_from_axis_deg(vec3d::Y, 45));
	u = t.get_global_up();
	f = t.get_global_forward();
	EXPECT_TRUE(u.is_near(0, 1, 0));
	EXPECT_TRUE(f.is_near(vec3(1, 0, 1).normalize()));
	EXPECT_TRUE(is_near(t.get_global_rotation().get_degrees(), 45, 0.0001));
}


TEST(transform, TranslateRotateParent) {
	transform p;
	p.set_pos(1, 0, 3);

	transform t;
	p.add_child(&t);

	EXPECT_EQ(t.get_pos(), vec3d::Zero);
	EXPECT_TRUE(t.get_global_pos().equals(1, 0, 3));

	t.set_global_pos(vec3(1, 0, 2));
	EXPECT_TRUE(t.get_global_pos().equals(1, 0, 2));
	EXPECT_TRUE(t.get_pos().equals(0, 0, -1));

	t.set_pos(1, 0, 0);
	EXPECT_TRUE(t.get_pos().equals(1, 0, 0));
	EXPECT_TRUE(t.get_global_pos().equals(2, 0, 3));

	// rotate
	p.set_rotate_y(90);
	EXPECT_TRUE(t.get_pos().equals(1, 0, 0));
	EXPECT_TRUE(t.get_global_pos().is_near(1, 0, 2));

	t.set_global_pos(vec3(1, 0, 2));
	EXPECT_TRUE(t.get_global_pos().is_near(1, 0, 2));
	EXPECT_TRUE(t.get_pos().is_within(1, 0, 0, 0.00001));

	t.set_global_pos(vec3(5, 2, 5));
	EXPECT_TRUE(t.get_global_pos().is_within(5, 2, 5, 0.000001));
}

TEST(transform, TranslateScaleParent) {
	transform p;
	p.set_pos(1, 2, 3);
	p.set_scale(10);

	transform t;
	p.add_child(&t);

	EXPECT_EQ(t.get_pos(), vec3d::Zero);
	EXPECT_TRUE(t.get_global_pos().equals(1, 2, 3));

	t.set_global_pos(vec3(2, 3, 4));
	const vec3 gp = t.get_global_pos();
	EXPECT_TRUE(t.get_global_pos().is_near(2, 3, 4));
	EXPECT_TRUE(t.get_pos().is_within(0.1, 0.1, 0.1, 0.000001));

	t.set_pos(1, 0, 0);
	EXPECT_TRUE(t.get_pos().equals(1, 0, 0));
	EXPECT_TRUE(t.get_global_pos().equals(11, 2, 3));
}

TEST(transform, TranslateParentRotateChild) {
	transform p;
	p.set_pos(1, 2, 3);

	transform t;
	p.add_child(&t);

	vec3 forward = t.get_global_forward();
	EXPECT_TRUE(forward.equals(0, 0, 1));

	t.set_rotate_y(90);
	forward = t.get_global_forward();
	EXPECT_TRUE(forward.is_near(1, 0, 0));
}


TEST(transform, ScaleParentRotateChild) {
	transform p;
	p.set_scale(2);

	transform t;
	p.add_child(&t);

	vec3 forward = t.get_global_forward();
	EXPECT_TRUE(forward.equals(0, 0, 1));

	t.set_rotate_y(90);

	matrix4 m = t.get_global_matrix();
	quat rotation = m.get_rotation();
	double d = rotation.get_around_y_degrees();
	EXPECT_TRUE(is_near(d, 90));

	forward = t.get_global_forward();
	EXPECT_TRUE(forward.is_near(1, 0, 0));
}


TEST(transform, ScaleRotateParentChild) {
	transform p;
	p.set_scale(2);
	p.set_rotate(quat(0, 1, 0, 0).normalize_self());

	transform t;
	p.add_child(&t);
	t.set_rotate(quat(0.5, 0, 0, 0.86).normalize_self());

	const vec3 forward = t.get_global_forward();
	EXPECT_TRUE(forward.is_within(0, -0.87, -0.49, 0.05));
}


TEST(transform, TranslateScaleRotateParent) {
	transform p;
	p.set_pos(1, 0, 3);
	p.set_scale(2);

	transform t;
	p.add_child(&t);

	EXPECT_EQ(t.get_pos(), vec3d::Zero);
	EXPECT_TRUE(t.get_global_pos().equals(1, 0, 3));

	t.set_global_pos(vec3(1, 0, 2));
	EXPECT_TRUE(t.get_global_pos().equals(1, 0, 2));
	EXPECT_TRUE(t.get_pos().equals(0, 0, -0.5));

	t.set_pos(1, 0, 0);
	EXPECT_TRUE(t.get_pos().equals(1, 0, 0));
	EXPECT_TRUE(t.get_global_pos().equals(3, 0, 3));

	// rotate
	p.set_rotate_y(90);
	EXPECT_TRUE(t.get_pos().equals(1, 0, 0));
	EXPECT_TRUE(t.get_global_pos().is_within(1, 0, 1, 0.00001));

	t.set_global_pos(vec3(1, 0, 2));
	EXPECT_TRUE(t.get_global_pos().is_near(1, 0, 2));
	EXPECT_TRUE(t.get_pos().is_within(0.5, 0, 0, 0.00001));

	t.set_global_pos(vec3(5, 2, 5));
	EXPECT_TRUE(t.get_global_pos().is_within(5, 2, 5, 0.000001));
}

//////////////////////////////////////////////////
// Advanced
//////////////////////////////////////////////////

TEST(transform, SetFromTransformParent) {
	transform t2;
	t2.set_global_pos(vec3(1, 2, 3));

	transform p;
	p.set_pos(-5, -6, -7);

	transform t;
	p.add_child(&t);
	t.set_global_from_global_transform(t2);
	EXPECT_TRUE(t.get_global_pos().equals(1, 2, 3));
	EXPECT_TRUE(t.get_pos().equals(6, 8, 10));
}


TEST(transform, TranslateParentParent) {
	transform p1;
	p1.set_pos(5, 6, 7);

	transform p2;
	p2.set_pos(1, 2, 3);
	p1.add_child(&p2);

	transform t;
	p2.add_child(&t);

	EXPECT_EQ(t.get_pos(), vec3d::Zero);
	EXPECT_TRUE(t.get_global_pos().equals(6, 8, 10));

	t.set_global_pos(vec3(1, 1, 1));
	EXPECT_TRUE(t.get_pos().equals(-5, -7, -9));
	EXPECT_TRUE(t.get_global_pos().equals(1, 1, 1));

	t.set_pos(vec3(2, 3, 4));
	EXPECT_TRUE(t.get_pos().equals(2, 3, 4));
	EXPECT_TRUE(t.get_global_pos().equals(8, 11, 14));

	t.set_global_pos(vec3(-4, -6, -8));
	EXPECT_TRUE(t.get_global_pos().equals(-4, -6, -8));

	p1.add_pos(1, 1, 1);
	EXPECT_TRUE(t.get_global_pos().equals(-3, -5, -7));
}


TEST(transform, TranslateRotateParentParent) {
	transform p1;
	p1.set_pos(5, 6, 7);
	p1.set_rotate_y(45);

	transform p2;
	p2.set_pos(1, 2, 3);
	p1.add_child(&p2);
	p2.set_rotate_y(45);

	transform t;
	p2.add_child(&t);

	t.set_global_pos(vec3(1, 1, 1));
	EXPECT_TRUE(t.get_global_pos().is_within(1, 1, 1, 0.0001));

	t.set_global_pos(vec3(-4, -6, -8));
	const vec3 gp = t.get_global_pos();
	EXPECT_TRUE(t.get_global_pos().is_within(-4, -6, -8, 0.0001));

	t.set_global_pos(vec3(1, 1, 1));
	EXPECT_TRUE(t.get_global_pos().is_within(1, 1, 1, 0.0001));
}


TEST(transform, SetFromTransformRotateParentParent) {
	transform p1;
	p1.set_pos(5, 6, 7);
	p1.set_rotate_y(45);

	transform p2;
	p2.set_pos(1, 2, 3);
	p2.set_rotate_y(45);
	p1.add_child(&p2);

	transform t2;
	t2.set_pos(1, 2, 3);
	t2.set_rotate_y(20);

	transform t;
	p2.add_child(&t);
	t.set_global_from_global_transform(t2);
	EXPECT_TRUE(t.get_global_pos().is_within(1, 2, 3, 0.00001));
	double d = t.get_global_rotation().get_degrees();
	EXPECT_TRUE(is_near(d, 20, 0.001));
}


TEST(transform, SetForward) {
	transform t;
	EXPECT_TRUE(t.get_global_forward().is_near(0, 0, 1));
	EXPECT_TRUE(t.get_global_up().is_near(0, 1, 0));
	EXPECT_TRUE(t.get_global_right().is_near(-1, 0, 0));

	// keeps up
	t.set_global_forward(vec3(0, 1, 1).normalize());
	EXPECT_TRUE(t.get_global_forward().is_near(0, 0, 1));
	EXPECT_TRUE(t.get_global_up().is_near(0, 1, 0));
	EXPECT_TRUE(t.get_global_right().is_near(-1, 0, 0));

	// keeps up (make sure NaN doesn't occur)
	t.set_global_forward(vec3(0, 1, 0).normalize());
	EXPECT_TRUE(t.get_global_forward().is_near(0, 0, 1));
	EXPECT_TRUE(t.get_global_up().is_near(0, 1, 0));
	EXPECT_TRUE(t.get_global_right().is_near(-1, 0, 0));

	// Keep up set forward
	vec3 forward = vec3(1, 1, 1).normalize();
	t.set_global_forward(forward);
	const vec3 u = t.get_global_up();
	const vec3 f = t.get_global_forward();
	EXPECT_TRUE(t.get_global_forward().is_near(vec3(1, 0, 1).normalize()));
	EXPECT_TRUE(t.get_global_up().is_near(0, 1, 0));
	EXPECT_TRUE(t.get_global_right().is_near(vec3(-1, 0, 1).normalize()));

	// Forward other dir
	forward = vec3(0, 0, -1).normalize();
	t.set_global_forward(forward);
	EXPECT_TRUE(t.get_global_forward().is_near(vec3(0, 0, -1)));
	EXPECT_TRUE(t.get_global_up().is_near(0, 1, 0));
	EXPECT_TRUE(t.get_global_right().is_near(vec3(1, 0, 0)));
}


TEST(transform, SetDir) {
	transform t;

	vec3 up = vec3(0, 1, 0).normalize();
	vec3 forward = vec3(0, 0, -1).normalize();
	t.set_global_dir(up, forward);
	EXPECT_TRUE(t.get_global_forward().is_near(0, 0, -1));
	EXPECT_TRUE(t.get_global_up().is_near(0, 1, 0));
	EXPECT_TRUE(t.get_global_right().is_near(1, 0, 0));

	up = vec3(1, 0, 1).normalize();
	forward = vec3(-1, 1, -1).normalize();
	t.set_global_dir(up, forward);

	EXPECT_TRUE(t.get_global_up().is_near(up));
	EXPECT_TRUE(t.get_global_forward().is_near(0, 1, 0));
	EXPECT_TRUE(t.get_global_right().is_near(t.get_global_forward().cross(t.get_global_up())));
}


TEST(transform, PlatformExampleWithoutUpAlign) {
	// platform
	transform platform;
	platform.rotate(set_from_axis_deg(vec3d::X, 45));

	// transform
	transform t;
	t.set_rotate_y(45);
	const vec3 forward_before = t.get_global_forward();
	const vec3 up_before = t.get_global_up();
	const vec3 right_before = t.get_global_right();
	double before = t.get_global_rotation().get_degrees();

	// inherit - but keep the same global as now
	transform keep = t.copy_from_global();
	platform.add_child(&t);
	t.set_global_from_global_transform(keep);

	// make sure we have the same global stuff
	const vec3 forward_after = t.get_global_forward();
	const vec3 up_after = t.get_global_up();
	const vec3 right_after = t.get_global_right();
	EXPECT_TRUE(forward_after.is_near(forward_before));
	EXPECT_TRUE(up_after.is_near(up_before));
	EXPECT_TRUE(right_after.is_near(right_before));

	double after = t.get_global_rotation().get_degrees();
	EXPECT_TRUE(is_near(after, before, 0.0001));

	t.rotate(set_from_axis_deg(up_after, -45));
	const vec3 forward_same_as_platform = t.get_global_forward();
	EXPECT_TRUE(forward_same_as_platform.is_near(0, 0, 1));
}


TEST(transform, PlatformExampleWithUpAlign) {
	// platform
	transform platform;
	platform.rotate(set_from_axis_deg(vec3d::X, 45));
	const vec3 platform_up = platform.get_global_up();
	const vec3 platform_forward = platform.get_global_forward();

	// transform
	transform t;
	t.set_rotate_y(45);
	const vec3 forward_before = t.get_global_forward();
	const vec3 up_before = t.get_global_up();

	// inherit - but keep the same global as now, but align up and right vectors
	transform keep = t.copy_from_global();
	platform.add_child(&t);
	t.set_global_from_global_transform(keep);

	const vec3 up_after_same = t.get_global_up();
	EXPECT_TRUE(up_before.is_near(up_after_same));

	t.set_global_dir(platform.get_global_up(), forward_before);

	// make sure we have the same global stuff
	const vec3 up_after = t.get_global_up();
	const vec3 forward_after = t.get_global_forward();
	EXPECT_TRUE(up_after.is_near(platform_up));
	//EXPECT_TRUE(forwardAfter.isNear(forwardBefore));

	//        t.rotate(Quat(upAfter, -45));
	//        const Vec3 forwardSameAsPlatform = vec3d::Z.rotate(t.getGlobalRotation());
	//        EXPECT_TRUE(forwardSameAsPlatform.isNear(platformForward));
}

//////////////////////////////////////////////////
// To local
//////////////////////////////////////////////////

TEST(transform, ToLocalPos) {
	transform t;
	EXPECT_TRUE(t.to_local_pos(0, 0, 0).is_near(0, 0, 0));
	EXPECT_TRUE(t.to_local_pos(1, 0, 0).is_near(1, 0, 0));
	EXPECT_TRUE(t.to_local_pos(0, 1, 0).is_near(0, 1, 0));
	EXPECT_TRUE(t.to_local_pos(0, 0, 1).is_near(0, 0, 1));

	t.set_pos(1, 0, 0);
	EXPECT_TRUE(t.to_local_pos(0, 0, 0).is_near(-1, 0, 0));
	EXPECT_TRUE(t.to_local_pos(1, 0, 0).is_near(0, 0, 0));
	EXPECT_TRUE(t.to_local_pos(0, 1, 0).is_near(-1, 1, 0));
	EXPECT_TRUE(t.to_local_pos(0, 0, 1).is_near(-1, 0, 1));

	t.set_pos(0, 0, 0);
	t.set_rotate_y(90);
	EXPECT_TRUE(t.to_local_pos(0, 0, 0).is_near(0, 0, 0));
	EXPECT_TRUE(t.to_local_pos(1, 0, 0).is_near(0, 0, 1));
	EXPECT_TRUE(t.to_local_pos(0, 1, 0).is_near(0, 1, 0));
	EXPECT_TRUE(t.to_local_pos(0, 0, 1).is_near(-1, 0, 0));

	t.set_rotate_y(0);
	t.set_scale(10);
	EXPECT_TRUE(t.to_local_pos(0, 0, 0).is_near(0, 0, 0));
	EXPECT_TRUE(t.to_local_pos(1, 0, 0).is_near(.1, 0, 0));
	EXPECT_TRUE(t.to_local_pos(0, 1, 0).is_near(0, .1, 0));
	EXPECT_TRUE(t.to_local_pos(0, 0, 1).is_near(0, 0, .1));

	t.set_rotate_y(90);
	t.set_scale(10);
	EXPECT_TRUE(t.to_local_pos(0, 0, 0).is_near(0, 0, 0));
	EXPECT_TRUE(t.to_local_pos(1, 0, 0).is_near(0, 0, .1));
	EXPECT_TRUE(t.to_local_pos(0, 1, 0).is_near(0, .1, 0));
	EXPECT_TRUE(t.to_local_pos(0, 0, 1).is_near(-.1, 0, 0));

	t.set_pos(1, 0, 0);
	t.set_rotate_y(90);
	t.set_scale(10);
	EXPECT_TRUE(t.to_local_pos(0, 0, 0).is_near(0, 0, -.1));
	EXPECT_TRUE(t.to_local_pos(1, 0, 0).is_near(0, 0, 0));
	EXPECT_TRUE(t.to_local_pos(0, 1, 0).is_near(0, .1, -.1));
	EXPECT_TRUE(t.to_local_pos(0, 0, 1).is_near(-.1, 0, -.1));
}
*/
}
