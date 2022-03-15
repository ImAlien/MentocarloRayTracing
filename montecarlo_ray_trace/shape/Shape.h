#pragma once

#include<glm/vec3.hpp>
#include<glm/glm.hpp>
#include "../ray/IntersectResult.h"
#include"../ray/Ray.h"
#include"../material/Material.h"
#include <vector>
class Point {
public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 tex;
	Point() {};
};
class Shape {
public:
	std::vector<Point*> points;
	Material* material;
	Shape() {};
	virtual IntersectResult intersect(Ray ray) {
		return IntersectResult();
	}
};
// 三角形
class Triangle : public Shape
{
public:
	glm::vec3 p1, p2, p3;    // 三顶点
	Triangle() {}
	Triangle(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, glm::vec3 color)
	{
		p1 = P1, p2 = P2, p3 = P3;
		material.normalVector = glm::normalize(glm::cross(p2 - p1, p3 - p1));
		material.color = color;
	}
	// 与光线求交
	IntersectResult intersect(Ray ray);
};