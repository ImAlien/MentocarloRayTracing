#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
class Texture {
public:
	int w, h;
	std::vector<std::vector<glm::vec3>> tex;
	Texture() {};
	Texture(std::string filename);
	glm::vec3 get(int i, int j);
	glm::vec3 get(float x, float y);
};
