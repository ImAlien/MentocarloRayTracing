#pragma once
#include"DataFrame.h"
#include <glm/glm.hpp>
#include <string>
#include<iostream>
#include"../test/Log.h"


class Camera {
public:
	std::string type;
	glm::vec3 eye;
	glm::vec3 lookat;
	glm::vec3 up;
	double fovy;
	int width, height;
	Camera();
	Camera(std::string scenename);
	Camera(std::string type_, glm::vec3 eye_, glm::vec3 lookat, glm::vec3 up,
		double foxy_, int w_, int h_);
	void bedroomCamera();
	void cornellboxCamera();
	void veachmisCamera();
	void defaultCamera();
	void saveImage(DataFrame* df);
	void simpleProcess(DataFrame* df);
	friend std::ostream &operator<<(std::ostream& os, const Camera& r) {
		os << "[eye:" << r.eye << "lookat" << r.lookat <<  "]\n";
		return os;
	}
};