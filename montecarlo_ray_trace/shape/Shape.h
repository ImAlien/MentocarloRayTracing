#pragma once

#include<glm/vec3.hpp>
#include<glm/glm.hpp>
#include"../ray/Ray.h"
#include"../material/Material.h"
#include "../ray/IntersectResult.h"
#include "../material/Texture.h"
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
	Material material;
	Shape() {};
	virtual std::shared_ptr<IntersectResult> intersect(Ray& ray) {
		return nullptr;
	}
};
// 三角形
class Triangle : public Shape
{
public:
	glm::vec3 p1, p2, p3;    // 三顶点
	glm::vec3 n1, n2, n3;
	glm::vec2 tex1, tex2, tex3;
	glm::vec3 normal;
	Triangle() {}
	Triangle(Shape* s);
	std::shared_ptr<IntersectResult> intersect(Ray& ray);
	glm::vec3 getKd();
	glm::vec3 getIntensity();
	glm::vec3 getTex(glm::vec3 p, Texture* tex);
};

