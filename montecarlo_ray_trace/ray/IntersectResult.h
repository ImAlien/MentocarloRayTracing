
#pragma once
#include "../main.h"
#include "../shape/Shape.h"
class Shape;
class Triangle;
class IntersectResult {
public:
	Triangle* triangle;
	bool isIntersect = false;             // 相交
	float distance = INF;         // 与交点的距离
	float alpha, beta, gama;
	glm::vec3 intersectPoint = glm::vec3(0, 0, 0);  // 光线交点
	//Material material;              // 交点的表面材质
	IntersectResult() {};
};
