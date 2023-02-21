
#include "Texture.h"
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace glm;

vec3 Texture::get(float x, float y) {
	if (x > 1 || y > 1) {
		cerr <<"�����������1";
	}
	if (x < 0 || y < 0) {
		cerr << "��������С��0";
	}
	int i = (1.0-y) * (h-1);
	int j = x * (w-1);
	return get(i, j);
}
vec3 Texture::get(int i, int j) {
	vec3 res;
	try {
		res = tex[i*w +j];
	}
	catch ( ... ) {
		cerr << "��ȡ����ʧ��" << endl;
		cout <<' ' << i << ' ' <<j << endl;
	}
	return res;
}
Texture::Texture(string filename) {
	cv::Mat image;
	image = cv::imread(filename);
	image = cv::imread(filename);
	if (image.data == nullptr)//nullptr��c++11�³��ֵĿ�ָ�볣��
	{
		cerr << "ͼƬ�ļ�������" << endl;
		exit(1);
	}
	w = image.cols;
	h = image.rows;
	tex = new vec3[w*h];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			/*float x = tr->tex1.x - (int)tr->tex1.x;
			float y = tr->tex1.y - (int)tr->tex1.y;
			int pic_x = image.rows * y;
			int pic_y = image.cols * x;*/
			//�õ������rbg��Ϣ
			int intb = image.at<cv::Vec3b>(i, j)[0];
			int intg = image.at<cv::Vec3b>(i, j)[1];
			int intr = image.at<cv::Vec3b>(i, j)[2];
			float b = intb * 1.0 / 255;
			float g = intg * 1.0 / 255;
			float r = intr * 1.0 / 255;
			b = powf(b, 2.2);
			g = powf(g, 2.2);
			r = powf(r, 2.2);
			tex[i*w +j] = vec3(r, g, b);
		}
	}
}