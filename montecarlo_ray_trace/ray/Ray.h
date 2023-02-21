#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
class Ray {
public:
	glm::vec3 startPoint;
	glm::vec3 direction,direction_inv;
	Ray() {};
	Ray(glm::vec3 p1, glm::vec3 p2):startPoint(p1),direction(p2 - p1) {
		direction_inv = glm::vec3(1.0 / direction.x, 1.0 / direction.y, 1.0 /direction.z);
	}
	void cutAlittle();
};

 Ray randomHemisphereRay(glm::vec3 N, glm::vec3 p);
glm::vec3 reflect(Ray& r, glm::vec3& N);
Ray reflect(Ray&r, glm::vec3& N, glm::vec3& p);
glm::vec3 refract(glm::vec3& N, glm::vec3& I, float n1, float n2);
Ray refract(Ray&r, glm::vec3& N, glm::vec3&p, float n1, float n2);
float shlick(glm::vec3 N, glm::vec3 I, float n1, float n2);