#pragma once
#include"DataFrame.h"
#include <glm/glm.hpp>
#include <string>


class Camera {
public:
	std::string type;
	glm::vec3 eye;
	glm::vec3 lookat;
	glm::vec3 up;
	double fovy;
	int width, height;
	Camera(std::string scenename);
	Camera(std::string type_, glm::vec3 eye_, glm::vec3 lookat, glm::vec3 up,
		double foxy_, int w_, int h_);
	void bedroomCamera();
	void cornellboxCamera();
	void veachmisCamera();
	void defaultCamera();
	void saveImage(DataFrame* df);
};