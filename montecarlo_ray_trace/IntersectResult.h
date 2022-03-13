#pragma once
#include<glm/vec3.hpp>
#include"Material.h"
class IntersectResult {
public:
	bool isIntersect = false;             // 相交
	double distance = 0.0f;         // 与交点的距离
	glm::vec3 intersectPoint = glm::vec3(0, 0, 0);  // 光线交点
	Material material;              // 交点的表面材质
	IntersectResult() {};
};
