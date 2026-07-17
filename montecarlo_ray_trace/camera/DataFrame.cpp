#include"DataFrame.h"
#include<iostream>
#include<glm/glm.hpp>
#include<fstream>
#include<cmath>
#include<vector>
#include<algorithm>

using namespace std;
using namespace glm;
DataFrame::DataFrame(int w, int h) {
	this->HEIGHT = h;
	this->WIDTH = w;
	this->data = new double[w*h*3];
	this->image = new unsigned char[w*h * 3];
	this->accum = new double[w*h*3];
	std::memset(accum, 0, sizeof(double) * WIDTH * HEIGHT * 3);
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

static double luminance(const double* data, int index) {
	return 0.2126 * data[index] + 0.7152 * data[index + 1] + 0.0722 * data[index + 2];
}

void DataFrame::removeOutliers(int radius, double threshold) {
	if (radius <= 0 || threshold <= 1.0) {
		return;
	}

	std::vector<double> source(data, data + WIDTH * HEIGHT * 3);
	std::vector<double> lumValues;
	std::vector<double> rValues;
	std::vector<double> gValues;
	std::vector<double> bValues;
	const int capacity = (radius * 2 + 1) * (radius * 2 + 1);
	lumValues.reserve(capacity);
	rValues.reserve(capacity);
	gValues.reserve(capacity);
	bValues.reserve(capacity);

	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			lumValues.clear();
			rValues.clear();
			gValues.clear();
			bValues.clear();

			for (int dy = -radius; dy <= radius; ++dy) {
				const int y = i + dy;
				if (y < 0 || y >= HEIGHT) continue;

				for (int dx = -radius; dx <= radius; ++dx) {
					const int x = j + dx;
					if (x < 0 || x >= WIDTH) continue;
					if (dx == 0 && dy == 0) continue;

					const int sample = (y * WIDTH + x) * 3;
					lumValues.push_back(luminance(source.data(), sample));
					rValues.push_back(source[sample]);
					gValues.push_back(source[sample + 1]);
					bValues.push_back(source[sample + 2]);
				}
			}

			if (lumValues.empty()) continue;

			const size_t mid = lumValues.size() / 2;
			std::nth_element(lumValues.begin(), lumValues.begin() + mid, lumValues.end());
			double medianLum = lumValues[mid];

			std::vector<double> deviations;
			deviations.reserve(lumValues.size());
			for (double value : lumValues) {
				deviations.push_back(std::fabs(value - medianLum));
			}
			std::nth_element(deviations.begin(), deviations.begin() + deviations.size() / 2, deviations.end());
			const double mad = deviations[deviations.size() / 2];
			const double limit = medianLum + threshold * glm::max(mad * 1.4826, 0.01);

			const int center = (i * WIDTH + j) * 3;
			const double centerLum = luminance(source.data(), center);
			if (centerLum > limit && centerLum > medianLum * threshold) {
				std::nth_element(rValues.begin(), rValues.begin() + rValues.size() / 2, rValues.end());
				std::nth_element(gValues.begin(), gValues.begin() + gValues.size() / 2, gValues.end());
				std::nth_element(bValues.begin(), bValues.begin() + bValues.size() / 2, bValues.end());
				data[center] = rValues[rValues.size() / 2];
				data[center + 1] = gValues[gValues.size() / 2];
				data[center + 2] = bValues[bValues.size() / 2];
			}
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