#pragma once
#include <glm/glm.hpp>
#include "../shape/Shape.h"
class Light {
public:
	glm::vec3 center;
	glm::vec3 intensity;
	glm::vec3 normal;
	glm::vec3 p1, p2, p3;
	float area;
	float pdf_light;
	Light();
	Light(Triangle* tr);
	glm::vec3 randomPoint();
};
