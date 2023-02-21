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
			*p++ = (unsigned char)glm::clamp((*S++) * 255, 0.0, 255.0);  // R ͨ��
			*p++ = (unsigned char)glm::clamp((*S++) * 255, 0.0, 255.0);  // G ͨ��
			*p++ = (unsigned char)glm::clamp((*S++) * 255, 0.0, 255.0);  // B ͨ��
		}
	}
}
void DataFrame::load(string file) {
	// 1. ��ͼƬ�ļ�
	ifstream is(file, ifstream::in | ios::binary);
	// 2. ����ͼƬ����
	is.seekg(0, is.end);  //���ļ���ָ�붨λ������ĩβ
	int length = is.tellg();
	is.seekg(0, is.beg);  //���ļ���ָ�����¶�λ�����Ŀ�ʼ
	// 3. �����ڴ滺����
	buffer = new char[length];
	// 4. ��ȡͼƬ
	is.read(buffer, length);
	is.close();
}