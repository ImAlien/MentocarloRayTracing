#include"DataFrame.h"
#include<iostream>
#include<glm/glm.hpp>

using namespace std;
DataFrame::DataFrame(int w, int h) {
	this->HEIGHT = h;
	this->WIDTH = w;
	this->data = new double[w*h*3];
	this->image = new unsigned char[w*h * 3];
	std::memset(data, 0.0, sizeof(double) * WIDTH * HEIGHT * 3);
}
void DataFrame::inputImage() {
	double* S = this->data;
	unsigned char* p = this->image;
	for (int i = 0; i < this->HEIGHT; i++)
	{
		for (int j = 0; j < this->WIDTH; j++)
		{
			*p++ = (unsigned char)glm::clamp((*S++) * 255, 0.0, 255.0);  // R 通道
			*p++ = (unsigned char)glm::clamp((*S++) * 255, 0.0, 255.0);  // G 通道
			*p++ = (unsigned char)glm::clamp((*S++) * 255, 0.0, 255.0);  // B 通道
		}
	}
}