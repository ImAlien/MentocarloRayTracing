#include"DataFrame.h"
#include<iostream>
#include<glm/glm.hpp>
#include<fstream>
#include<cmath>
#include<vector>

using namespace std;
using namespace glm;
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

void DataFrame::bilateralFilter(int radius, double sigmaSpace, double sigmaColor) {
	if (radius <= 0 || sigmaSpace <= 0.0 || sigmaColor <= 0.0) {
		return;
	}

	std::vector<double> source(data, data + WIDTH * HEIGHT * 3);
	const double invSpace = 1.0 / (2.0 * sigmaSpace * sigmaSpace);
	const double invColor = 1.0 / (2.0 * sigmaColor * sigmaColor);

	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			const int center = (i * WIDTH + j) * 3;
			double sumWeight = 0.0;
			double color[3] = { 0.0, 0.0, 0.0 };

			for (int dy = -radius; dy <= radius; ++dy) {
				const int y = i + dy;
				if (y < 0 || y >= HEIGHT) continue;

				for (int dx = -radius; dx <= radius; ++dx) {
					const int x = j + dx;
					if (x < 0 || x >= WIDTH) continue;

					const int sample = (y * WIDTH + x) * 3;
					const double dr = source[sample] - source[center];
					const double dg = source[sample + 1] - source[center + 1];
					const double db = source[sample + 2] - source[center + 2];
					const double spaceDistance2 = static_cast<double>(dx * dx + dy * dy);
					const double colorDistance2 = dr * dr + dg * dg + db * db;
					const double weight = std::exp(-spaceDistance2 * invSpace - colorDistance2 * invColor);

					sumWeight += weight;
					color[0] += source[sample] * weight;
					color[1] += source[sample + 1] * weight;
					color[2] += source[sample + 2] * weight;
				}
			}

			if (sumWeight > 0.0) {
				data[center] = color[0] / sumWeight;
				data[center + 1] = color[1] / sumWeight;
				data[center + 2] = color[2] / sumWeight;
			}
		}
	}
}
void DataFrame::load(string file) {
	// 1. 打开图片文件
	ifstream is(file, ifstream::in | ios::binary);
	// 2. 计算图片长度
	is.seekg(0, is.end);  //将文件流指针定位到流的末尾
	int length = is.tellg();
	is.seekg(0, is.beg);  //将文件流指针重新定位到流的开始
	// 3. 创建内存缓存区
	buffer = new char[length];
	// 4. 读取图片
	is.read(buffer, length);
	is.close();
}