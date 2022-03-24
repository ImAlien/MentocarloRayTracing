#pragma once
#include<string>
class DataFrame {
public:
	int WIDTH, HEIGHT;
	double* data;
	unsigned char* image;
	char* buffer;
	DataFrame(int w, int h);
	void inputImage();
	void load(std::string file);
};