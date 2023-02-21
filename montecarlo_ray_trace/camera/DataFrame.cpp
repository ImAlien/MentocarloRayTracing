#include"DataFrame.h"
#include<iostream>
#include<glm/glm.hpp>
#include<fstream>

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