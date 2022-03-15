#pragma once

class DataFrame {
public:
	int WIDTH, HEIGHT;
	double* data;
	unsigned char* image;
	DataFrame(int w, int h);
	void inputImage();
};