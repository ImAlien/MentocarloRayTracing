#pragma once
#include<glm/glm.hpp>
#include <algorithm>
#include<math.h>
#include "../shape/Shape.h"
class BoundingBox {
public:
	glm::vec3 aa, bb;
	Triangle* source = nullptr;
	BoundingBox() {};
	BoundingBox(Triangle* tr);
	BoundingBox(glm::vec3 p1, glm::vec3 p2);
	bool intersectBB(Ray& ray);
};

bool cmpBBx(const BoundingBox &a, const BoundingBox &b);
bool cmpBBy(const BoundingBox &a, const BoundingBox &b);
bool cmpBBz(const BoundingBox &a, const BoundingBox &b);