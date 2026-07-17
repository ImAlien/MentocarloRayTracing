#pragma once
#include<string>
class DataFrame {
public:
	int WIDTH, HEIGHT;
	double* data;          // display buffer: gamma-corrected, [0,1]
	double* accum;         // progressive accumulation buffer: linear radiance sums
	unsigned char* image;
	char* buffer;
	DataFrame(int w, int h);
	void inputImage();
	void removeOutliers(int radius, double threshold);
	void bilateralFilter(int radius, double sigmaSpace, double sigmaColor);
	void load(std::string file);
};