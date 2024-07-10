#include "afront/volume_util.h"

#include <gtest/gtest.h>

#include "test_models.h"
#include "core/util/timer.h"

namespace playchilla {
struct surface_result {
	double test_distance = 0;
	int hits = 0;
	int misses = 0;
	double abs_surface_error_sum = 0;
	double distance_from_start_sum = 0;
	uint64_t time_ms = 0;
};

inline surface_result test_find_surface(const volume* v, int attempts, double test_distance, const std::function<std::optional<vec3>(const volume*, const vec3&)>& f) {
	const timer t;
	mx3::random r(123);
	surface_result result{test_distance};
	for (int i = 0; i < attempts; ++i) {
		auto dir = vec3d::create_random_dir(r);
		const auto pos = dir * test_distance;
		if (auto sp = f(v, pos)) {
			result.abs_surface_error_sum += std::abs(v->get_value(*sp));
			result.distance_from_start_sum += pos.distance(*sp);
			++result.hits;
		}
		else {
			auto q = f(v, pos);
			++result.misses;
		}
	}
	result.time_ms = t.millie_seconds();
	return result;
}

using test_function = std::function<std::optional<vec3>(const volume*, const vec3&)>;

struct test_config {
	std::string name;
	const volume* v;
	double size;
	double test_radius;
	test_function f;
};


inline void print(const surface_result& result) {
	std::cout
		<< "--------------------------------\n"
		<< "test distance: " << result.test_distance << "\n"
		<< "misses       : " << result.misses << "\n"
		<< "error/hit    : " << result.abs_surface_error_sum / result.hits << "\n"
		<< "distance/hit : " << result.distance_from_start_sum / result.hits << "\n"
		<< "time         : " << result.time_ms << "ms\n"
		<< "\n\n";
	std::cout << std::flush;
}

std::vector<surface_result> run_surface_tests(const test_function& f) {
	constexpr int attempts = 10000;
	constexpr double size = 100;
	csg csg(123);
	const auto* v = test::create_noisy_planet(csg, size);

	std::vector<surface_result> r;
	r.push_back(test_find_surface(v, attempts, 1., f));
	r.push_back(test_find_surface(v, attempts, .5 * size, f));
	r.push_back(test_find_surface(v, attempts, size, f));
	r.push_back(test_find_surface(v, attempts, 1.5 * size, f));
	r.push_back(test_find_surface(v, attempts, 2. * size, f));
	r.push_back(test_find_surface(v, attempts, 10. * size, f));
	r.push_back(test_find_surface(v, attempts, 100. * size, f));

	auto total_misses = 0;
	for (const auto& result : r) {
		//print(result);
		total_misses += result.misses;
	}
	//std::cout << "--------------------------------\n";
	//std::cout << "total misses: " << total_misses;
	return r;
}

TEST(volume_util, FindSurfaceQualityAlongRay) {
	const auto f = [](const volume* v, const vec3& pos) {
		return find_surface_along_ray(v, pos, pos.normalize(), .1);
	};
	run_surface_tests(f);
}

//TEST(volume_util, FindSurfaceQualityOriginal) {
//	const auto f = [](const volume* v, const vec3& pos) {
//		return find_surface(v, pos, 0.5, 3000);
//	};
//	run_surface_tests(f);
//}

TEST(volume_util, CalcNormalBasic) {
	csg csg(12345);

	const auto* source = csg.cube().get();

	EXPECT_FALSE(calc_normal(source, vec3(-2, 0, 0), 0));

	auto n = calc_normal(source, vec3(-2, 0, 0), 1);
	EXPECT_TRUE(n->distance(-1, 0, 0) < 0.001);

	n = calc_normal(source, vec3(2, 0, 0), 1);
	EXPECT_TRUE(n->distance(1, 0, 0) < 0.001);

	n = calc_normal(source, vec3(0, 2, 0), 1);
	EXPECT_TRUE(n->distance(0, 1, 0) < 0.001);

	n = calc_normal(source, vec3(0, -2, 0), 1);
	EXPECT_TRUE(n->distance(0, -1, 0) < 0.001);

	n = calc_normal(source, vec3(0, 0, 2), 1);
	EXPECT_TRUE(n->distance(0, 0, 1) < 0.001);

	n = calc_normal(source, vec3(0, 0, -2), 1);
	EXPECT_TRUE(n->distance(0, 0, -1) < 0.001);
}


TEST(volume_util, CalcNormalTwoCubes) {
	csg csg(12345);

	// -3 -> -1
	std::vector<const volume*> volumes = {
		csg.cube(),
		csg.cube().translate({-4, 0, 0})
	};

	const auto both = std::make_unique<volumes::union_volume>(volumes);

	const auto* source = both.get();

	EXPECT_FALSE(calc_normal(source, vec3(-2, 0, 0), 1));

	auto n = calc_normal(source, vec3(-2.5, 0, 0), 1);
	EXPECT_TRUE(n->distance(1, 0, 0) < 0.001);

	n = calc_normal(source, vec3(-1.5, 0, 0), 1);
	EXPECT_TRUE(n->distance(-1, 0, 0) < 0.001);
}

TEST(volume_util, FindSurfaceBasic) {
	csg csg(12345);
	const auto* source = csg.cube({2, 2, 2}).get();

	auto sp = find_surface(source, vec3(-2, 0, 0), 0.01, 10);
	EXPECT_TRUE(sp->distance(-1, 0, 0) <= 0.011);

	sp = find_surface(source, vec3(0, 2, 0), 0.01, 10);
	EXPECT_TRUE(sp->distance(0, 1, 0) <= 0.011);

	sp = find_surface(source, vec3(0, 0, -2), 0.01, 10);
	EXPECT_TRUE(sp->distance(0, 0, -1) <= 0.011);
}

TEST(volume_util, FindSurfaceTwoCubes) {
	csg csg(12345);
	const auto* both = csg.unions({csg.cube({2, 2, 2}), csg.cube({2, 2, 2}).translate(vec3(-3, 0, 0))}).get();
	const auto* source = both;

	auto sp = find_surface(source, vec3(-1.8, 0, 0), 0.01, 1);
	EXPECT_TRUE(sp->distance(-2, 0, 0) < 0.01);

	sp = find_surface(source, vec3(-1.2, 0, 0), 0.01, 1);
	EXPECT_TRUE(sp->distance(-1, 0, 0) < 0.01);
}

TEST(volume_util, FindSurfaceScaled) {
	csg csg(12345);

	const auto* source = csg.sphere(1).scale(2).get();

	// inside around center
	EXPECT_NEAR(source->get_value({0,0,0}), 2, Epsilon);
	EXPECT_NEAR(source->get_value({-1,0,0}), 1, Epsilon);
	EXPECT_NEAR(source->get_value({1,0,0}), 1, Epsilon);
	EXPECT_NEAR(source->get_value({0,-1,0}), 1, Epsilon);
	EXPECT_NEAR(source->get_value({0,1,0}), 1, Epsilon);
	EXPECT_NEAR(source->get_value({0,0,1}), 1, Epsilon);
	EXPECT_NEAR(source->get_value({0,0,1}), 1, Epsilon);
	EXPECT_NEAR(source->get_value({2,0,0}), 0, Epsilon);
	EXPECT_NEAR(source->get_value({0,2,0}), 0, Epsilon);
	EXPECT_NEAR(source->get_value({0,0,2}), 0, Epsilon);
	EXPECT_NEAR(source->get_value({3,0,0}), -1, Epsilon);
	EXPECT_NEAR(source->get_value({0,3,0}), -1, Epsilon);
	EXPECT_NEAR(source->get_value({0,0,3}), -1, Epsilon);

	{
		const auto pos = vec3(0.5, 0, 0);
		const vec3 n = *calc_normal(source, pos, 0.01);
		const double v = source->get_value(pos);

		const vec3 tp = pos + n * v;
		const double vs = source->get_value(tp);
		EXPECT_TRUE(std::abs(vs) < Epsilon);
	}

	{
		const auto pos = vec3(0, 0.5, 0);
		const vec3 n = *calc_normal(source, pos, 0.01);
		const double v = source->get_value(pos);
		//EXPECT_TRUE(v < 1); // 0.25 -> 1 (0.5 -> 2) => 1.5??

		const vec3 tp = pos + n * v;
		double vs = source->get_value(tp);
		//EXPECT_TRUE(Math.abs(vs) < Vec3.Epsilon);
	}
}
}
