
#pragma once
#include "../main.h"
#include "../shape/Shape.h"
class Shape;
class Triangle;
class IntersectResult {
public:
	Triangle* triangle;
	bool isIntersect = false;             // �ཻ
	float distance = INF;         // �뽻��ľ���
	float alpha, beta, gama;
	glm::vec3 intersectPoint = glm::vec3(0, 0, 0);  // ���߽���
	//Material material;              // ����ı������
	IntersectResult() {};
};
