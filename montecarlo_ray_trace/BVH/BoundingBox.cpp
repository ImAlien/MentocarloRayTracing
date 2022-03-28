
#include"BoundingBox.h"

using namespace std;
BoundingBox::BoundingBox(Triangle* tr) {
	aa.x = min(min(tr->p1.x, tr->p2.x), tr->p3.x);
	aa.y = min(min(tr->p1.y, tr->p2.y), tr->p3.y);
	aa.z = min(min(tr->p1.z, tr->p2.z), tr->p3.z);
	bb.x = max(max(tr->p1.x, tr->p2.x), tr->p3.x);
	bb.y = max(max(tr->p1.y, tr->p2.y), tr->p3.y);
	bb.z = max(max(tr->p1.z, tr->p2.z), tr->p3.z);
	source = tr;
}
BoundingBox::BoundingBox(glm::vec3 p1, glm::vec3 p2) {
	aa = p1;
	bb = p2;
}
float BoundingBox::intersectBB(Ray& r) {
	// 1.0 / direction
	glm::vec3 invdir = r.direction_inv;

	glm::vec3 in = (bb - r.startPoint) * invdir;
	glm::vec3 out = (aa - r.startPoint) * invdir;

	glm::vec3 tmax = max(in, out);
	glm::vec3 tmin = min(in, out);

	float t1 = min(tmax.x, min(tmax.y, tmax.z));
	float t0 = max(tmin.x, max(tmin.y, tmin.z));

	return (t1 >= t0) ? min(t1, t0): -1;
}
bool cmpBBx(const BoundingBox &a, const BoundingBox &b) {
	return a.aa.x < b.aa.x;
}
bool cmpBBy(const BoundingBox &a, const BoundingBox &b) {
	return a.aa.y < b.aa.y;
}
bool cmpBBz(const BoundingBox &a, const BoundingBox &b) {
	return a.aa.z < b.aa.z;
}
