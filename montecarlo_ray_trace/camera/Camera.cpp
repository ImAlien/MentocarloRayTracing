#include "Camera.h"
#include "../include/svpng.inc"
#include <iostream>



using namespace std;
void Camera::saveImage(DataFrame* df) {
	FILE* fp;
	fopen_s(&fp, "image.png", "wb");
	svpng(fp, df->WIDTH, df->HEIGHT, df->image, 0);
	fclose(fp);
}
Camera::Camera(string scenename) {
	if (scenename == "bedroom") {
		bedroomCamera();
	}
	else if (scenename == "veach_mis") {
		veachmisCamera();
	}
	else if (scenename == "cornell_box") {
		cornellboxCamera();
	}
	else {
		defaultCamera();
	}
}
Camera::Camera(std::string type_, glm::vec3 eye_, glm::vec3 lookat_, glm::vec3 up_,
	double foxy_, int w_, int h_) {
	this->type = type_;
	this->eye = eye_;
	this->lookat = lookat_;
	this->up = up_;
	this->fovy = foxy_;
	this->width = w_;
	this->height = h_;
}
void Camera::bedroomCamera() {
	new (this) Camera(
		"perspective",
		glm::vec3(3.456, 1.212, 3.299),
		glm::vec3(2.699, 1.195, 2.645),
		glm::vec3(-0.013, 1.000, -0.011),
		39.4305,
		1280,
		720
	);
}

void Camera::cornellboxCamera() {
	new (this) Camera(
		"perspective",
		glm::vec3(0, 1, 6.8),
		glm::vec3(0, 1, 5.8),
		glm::vec3(0, 1, 0),
		19.5,
		1024,
		1024
	);
}
void Camera::veachmisCamera() {
	new (this) Camera(
		"perspective",
		glm::vec3(0.0, 2.0, 15.0),
		glm::vec3(0.0, 1.69521, 14.0476),
		glm::vec3(0.0, 0.952421, -0.304787),
		27.3909,
		1200,
		900
	);
}
void Camera::defaultCamera() {
	new (this) Camera(
		"perspective",
		glm::vec3(0.0, 0.0, 6.0),
		glm::vec3(0.0, 0, 0),
		glm::vec3(0.0, 1, 0),
		45,
		800,
		600
	);
}