#include "Shape.h"

using namespace glm;
IntersectResult Triangle::intersect(Ray& ray) {
	IntersectResult res;

	vec3 S = ray.startPoint;        // 射线起点
	vec3 d = ray.direction;         // 射线方向
	vec3 N = normalize(cross(p2 - p1, p3 - p1));    // 法向量
	if (dot(N, d) > 0.0f) N = -N;   // 获取正确的法向量

	// 如果视线和三角形平行
	if (fabs(dot(N, d)) < 0.00001f) return res;

	// 距离
	float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
	if (t < 0.0005f) return res;    // 如果三角形在光线背面

	// 交点计算
	vec3 P = S + d * t;

	// 判断交点是否在三角形中
	vec3 c1 = cross(p2 - p1, P - p1);
	vec3 c2 = cross(p3 - p2, P - p2);
	vec3 c3 = cross(p1 - p3, P - p3);
	if (dot(c1, N) > 0 && dot(c2, N) > 0 && dot(c3, N) > 0) {
		res.distance = t;
		res.isIntersect = true;
		res.triangle = this;
		return res;
	}
	if (dot(c1, N) < 0 && dot(c2, N) < 0 && dot(c3, N) < 0) {
		res.distance = t;
		res.isIntersect = true;
		res.triangle = this;
		return res;
	}

	return res;
}