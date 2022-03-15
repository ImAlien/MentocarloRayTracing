#pragma once

#include <glm/vec3.hpp>
class Ray {
public:
	glm::vec3 startPoint;
	glm::vec3 direction;
	Ray() {};
};