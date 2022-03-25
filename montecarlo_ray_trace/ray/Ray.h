#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
class Ray {
public:
	glm::vec3 startPoint;
	glm::vec3 direction;
	Ray() {};
	Ray(glm::vec3 p1, glm::vec3 p2) {
		startPoint = p1;
		direction = p2 - p1;
	}
	void cutAlittle();
};

Ray randomHemisphereRay(glm::vec3 N, glm::vec3 p);