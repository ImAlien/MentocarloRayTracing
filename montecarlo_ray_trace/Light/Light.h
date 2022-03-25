#pragma once
#include <glm/glm.hpp>
#include "../shape/Shape.h"
class Light {
public:
	glm::vec3 center;
	glm::vec3 intensity;
	glm::vec3 normal;
	float area;
	float pdf_light;
	Light();
	Light(Triangle* tr);
};
