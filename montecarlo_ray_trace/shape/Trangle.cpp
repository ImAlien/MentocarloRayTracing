#include "Shape.h"
#include "../main.h"


using namespace std;
using namespace glm;
shared_ptr<IntersectResult> Triangle::intersect(Ray& ray) {
	shared_ptr<IntersectResult> res(new IntersectResult());

	vec3& O = ray.startPoint;        // 射线起点
	vec3& D = ray.direction;         // 射线方向
	vec3& N = normal;    // 法向量

	 // 如果视线和三角形平行
	if (fabs(dot(N, D)) < 0.00001f) return res;

	//// 距离
	//float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
	//if (t < 0.0005f) return res;    // 如果三角形在光线背面

	//// 交点计算
	//vec3 P = S + d * t;

	//// 判断交点是否在三角形中
	//vec3 c1 = cross(p2 - p1, P - p1);
	//vec3 c2 = cross(p3 - p2, P - p2);
	//vec3 c3 = cross(p1 - p3, P - p3);
	//if (dot(c1, N) >= 0 && dot(c2, N) >= 0 && dot(c3, N) >= 0) {
	//	res.distance = t;
	//	res.isIntersect = true;
	//	res.triangle = this;
	//	res.intersectPoint = P;
	//	return res;
	//}
	//if (dot(c1, N) <= 0 && dot(c2, N) <= 0 && dot(c3, N) <= 0) {
	//	res.distance = t;
	//	res.isIntersect = true;
	//	res.triangle = this;
	//	res.intersectPoint = P;
	//	return res;
	//}
	vec3 E1 = p2 - p1, E2 = p3 - p1;
	vec3 S = O - p1, S1 = cross(D, E2), S2 = cross(S, E1);
	float deno = dot(S1, E1);
	if (fabs(deno) < EPSILON) return res;
	float t = dot(S2, E2) / deno;
	float b1 = dot(S1, S) / deno;
	float b2 = dot(S2, D) / deno;
	if (t > EPSILON && b1 >= 0 && b2 >= 0 && (1 - b1 - b2) >= 0) {
		res->distance = t;
		res->intersectPoint = O + t * D;
		res->triangle = this;
		res->isIntersect = true;
		return res;
	}
	return res;
}
Triangle::Triangle(Shape* s) {
	this->material = s->material;
	p1 = s->points[0].pos;
	p2 = s->points[1].pos;
	p3 = s->points[2].pos;
	n1 = s->points[0].normal;
	n2 = s->points[1].normal;
	n3 = s->points[2].normal;
	tex1 = s->points[0].tex;
	tex2 = s->points[1].tex;
	tex3 = s->points[2].tex;
	normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
}
vec3 Triangle::getKd() {
	if (this->material.diffuse_texname == "") return material.Kd;
	
}
vec3 Triangle::getIntensity() {
	return this->material.getIntensity();
}

float Area(vec3 a, vec3 b, vec3 c) {
	return sqrt(dot(cross(b - a, c - a), cross(b - a, c - a)));
}
glm::vec3 Triangle::getTex(glm::vec3 p, Texture* texMap) {
	float A = Area(p1, p2, p3);
	float alpha = Area(p, p2, p3) / A;
	float beta = Area(p, p1, p3) / A;
	float gama = 1 - alpha - beta;
	float u = tex1.x * alpha + tex2.x*beta + tex3.x  * gama;
	float v = tex1.y * alpha + tex2.y*beta + tex3.y * gama;
	u = u - floor(u);
	v = v - floor(v);
	return texMap->get(u, v);
}