
#include "Texture.h"
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace glm;

vec3 Texture::get(float x, float y) {
	int i = x * h;
	int j = y * w;
	return get(i, j);
}
vec3 Texture::get(int i, int j) {
	return tex[i][j];
}
Texture::Texture(string filename) {
	cv::Mat image;
	image = cv::imread(filename);
	image = cv::imread(filename);
	if (image.data == nullptr)//nullptr是c++11新出现的空指针常量
	{
		cerr << "图片文件不存在" << endl;
		exit(1);
	}
	w = image.cols;
	h = image.rows;
	tex.resize(h);
	for (int i = 0; i < h; i++) tex[i].resize(w);
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			/*float x = tr->tex1.x - (int)tr->tex1.x;
			float y = tr->tex1.y - (int)tr->tex1.y;
			int pic_x = image.rows * y;
			int pic_y = image.cols * x;*/
			//得到纹理的rbg信息
			int intb = image.at<cv::Vec3b>(i, j)[0];
			int intg = image.at<cv::Vec3b>(i, j)[1];
			int intr = image.at<cv::Vec3b>(i, j)[2];
			float b = intb * 1.0 / 255;
			float g = intg * 1.0 / 255;
			float r = intr * 1.0 / 255;
			b = powf(b, 2.2);
			g = powf(g, 2.2);
			r = powf(r, 2.2);
			tex[i][j] = vec3(r, g, b);
		}
	}
}