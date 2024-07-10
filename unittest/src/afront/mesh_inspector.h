#pragma once
#include <cstdint>
#include <optional>
#include <vector>

#include "core/math/vec3.h"

namespace playchilla {
struct test_triangle {
	vec3 a;
	vec3 b;
	vec3 c;
	vec3 la;
	vec3 lb;
	vec3 lc;

	test_triangle(const vec3& a, const vec3& b, const vec3& c) : a(a), b(b), c(c) {
		constexpr double scale = 0.99;
		const vec3 center = (a + b + c) * (1. / 3.);
		la = (a - center) * scale + center;
		lb = (b - center) * scale + center;
		lc = (c - center) * scale + center;
	}
};

struct inspection_result {
	uint64_t total_intersections = 0;
};

class plane {
public:
	vec3 normal;
	double d;

	enum class side {
		OnPlane,
		Back,
		Front
	};


	plane(const vec3& a, const vec3& b, const vec3& c) {
		normal = (b - a).cross(c - a).normalize();
		d = -a.dot(normal);
	}

	side test_point(const vec3& point) const {
		const double dist = normal.dot(point) + d;
		if (dist == 0) {
			return side::OnPlane;
		}
		if (dist < 0) {
			return side::Back;
		}
		return side::Front;
	}
};

inline bool is_point_in_triangle(const vec3& point, const vec3& t1, const vec3& t2, const vec3& t3) {
	const vec3 v0 = t1 - point;
	const vec3 v1 = t2 - point;
	const vec3 v2 = t3 - point;

	const double ab = v0.dot(v1);
	const double ac = v0.dot(v2);
	const double bc = v1.dot(v2);
	const double cc = v2.dot(v2);

	if (bc * ac - cc * ab < 0) {
		return false;
	}
	const double bb = v1.dot(v1);
	if (ab * bc - ac * bb < 0) {
		return false;
	}
	return true;
}

inline std::optional<vec3> intersect_ray_triangle(const vec3& origin, const vec3& dir,
                                                  const vec3& t1, const vec3& t2, const vec3& t3) {
	const vec3 edge1 = t2 - t1;
	const vec3 edge2 = t3 - t1;
	const vec3 pvec = dir.cross(edge2);
	double det = edge1.dot(pvec);
	if (det < Epsilon) {
		const plane p(t1, t2, t3);
		if (p.test_point(origin) == plane::side::OnPlane && is_point_in_triangle(origin, t1, t2, t3)) {
			return origin;
		}
		return {};
	}

	det = 1.0 / det;

	const vec3 tvec = origin - t1;
	const double u = tvec.dot(pvec) * det;
	if (u < 0.0 || u > 1.0) {
		return {};
	}

	const vec3 qvec = tvec.cross(edge1);
	const double v = dir.dot(qvec) * det;
	if (v < 0.0 || u + v > 1.0) {
		return {};
	}

	const double t = edge2.dot(qvec) * det;
	if (t < 0) {
		return {};
	}

	if (t <= Epsilon) {
		return origin;
	}
	return origin + dir * t;
}

inline int ray_triangle(const vec3& a, const vec3& b, const test_triangle& t) {
	const vec3 ab = b - a;
	const double seg_len = ab.length();
	constexpr double scale = 0.99;

	double start_seg = seg_len * (1 - scale);
	double end_seg = seg_len * scale;
	if (auto ip = intersect_ray_triangle(a, ab.normalize(), t.la, t.lb, t.lc)) {
		double dist = ip->distance_sqr(a);
		if (dist < start_seg * start_seg) {
			return 0;
		}
		if (dist > end_seg * end_seg) {
			return 0;
		}
		return 1;
	}
	return 0;
}

inline int intersection(const test_triangle& t1, const test_triangle& t2) {
	int tot = ray_triangle(t1.a, t1.b, t2);
	tot += ray_triangle(t1.b, t1.a, t2);
	tot += ray_triangle(t1.a, t1.c, t2);
	tot += ray_triangle(t1.c, t1.a, t2);
	tot += ray_triangle(t1.b, t1.c, t2);
	tot += ray_triangle(t1.c, t1.b, t2);
	if (tot > 0) {
		//t2.t.tag = true;
		//t1.t.is = t2.t;
		//t1.t.ip = ip;
	}
	return tot;
}

inline inspection_result check(const std::vector<test_triangle>& triangles) {
	uint64_t total_intersection = 0;
	for (int i = 0; i < triangles.size(); ++i) {
		const test_triangle& ta = triangles[i];
		for (int j = i + 1; j < triangles.size(); ++j) {
			const test_triangle& tb = triangles[j];
			if (ta.a.distance_sqr(tb.a) > 50 * 50) {
				continue;
			}
			const int intersections = intersection(ta, tb);
			if (intersections > 0) {
				total_intersection += 1;
				break;
			}
		}
	}

	return {total_intersection};
}
}
