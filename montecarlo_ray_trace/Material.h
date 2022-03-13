#pragma once
#include<glm/vec3.hpp>
class Material {
public:
	bool isLightSource = false;
	glm::vec3 color = glm::vec3(0,0,0);
	glm::vec3 normalVector = glm::vec3(0,0,0);
	Material() {};
};