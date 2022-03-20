#pragma once

#include<glm/vec3.hpp>
#include<glm/glm.hpp>
#include"../ray/Ray.h"
#include"../material/Material.h"
#include "../ray/IntersectResult.h"
#include <vector>
#include "../main.h"
class Point {
public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 tex;
	Point() {};
};
class Shape {
public:
	std::vector<Point> points;
	Material* material;
	Shape() {};
	virtual IntersectResult intersect(Ray& ray) {
		IntersectResult res;
		return res;
	}
};
// 三角形
class Triangle : public Shape
{
public:
	glm::vec3 p1, p2, p3;    // 三顶点
	glm::vec3 normal;
	Triangle() {}
	Triangle(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3)
	{
		p1 = P1, p2 = P2, p3 = P3;
		normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
	}
	IntersectResult intersect(Ray& ray);
};

