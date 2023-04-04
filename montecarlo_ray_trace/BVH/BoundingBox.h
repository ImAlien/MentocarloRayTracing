#pragma once
#include<glm/glm.hpp>
#include <algorithm>
#include<math.h>
#include "../shape/Shape.h"
class BoundingBox {
public:
	glm::vec3 aa, bb, center;
	Triangle* source = nullptr;
	BoundingBox():aa(glm::vec3(100, 100, 100)),bb(glm::vec3(-100, -100, -100)){};
	BoundingBox(Triangle* tr);
	BoundingBox(glm::vec3 p1, glm::vec3 p2);
	float intersectBB(Ray& ray);
	glm::vec3 offset(BoundingBox& b) {
		glm::vec3 ans = b.bb - aa;
		ans.z /= (bb.z - aa.z);
		ans.x /= (bb.x - aa.x);
		ans.y /= (bb.y - aa.y);
		return ans;
	}
	void merge(BoundingBox& b) {
		aa.x = glm::min(aa.x, b.aa.x);
		aa.y = glm::min(aa.y, b.aa.y);
		aa.z = glm::min(aa.z, b.aa.z);
		bb.x = glm::max(bb.x, b.bb.x);
		bb.y = glm::max(bb.y, b.bb.y);
		bb.z = glm::max(bb.z, b.bb.z);
	}
	float area() {
		float lenx = bb.x - aa.x;
		float leny = bb.y - aa.y;
		float lenz = bb.z - aa.z;
		return 2 * (lenx * leny + lenx * lenz + leny * lenz);
	}
};

bool cmpBBx(const BoundingBox &a, const BoundingBox &b);
bool cmpBBy(const BoundingBox &a, const BoundingBox &b);
bool cmpBBz(const BoundingBox &a, const BoundingBox &b);