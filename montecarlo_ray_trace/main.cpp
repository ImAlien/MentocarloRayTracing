#include<glm/glm.hpp>
#include"svpng.inc"
#include <omp.h>    // openmp多线程加速
#include<glm/vec3.hpp>
#include<iostream>


void test_rgb(void) {
	unsigned char rgb[256 * 256 * 3], *p = rgb;
	unsigned x, y;
	FILE *fp = fopen("rgb.png", "wb");
	for (y = 0; y < 256; y++)
		for (x = 0; x < 256; x++) {
			*p++ = (unsigned char)x;    /* R */
			*p++ = (unsigned char)y;    /* G */
			*p++ = 128;                 /* B */
		}
	svpng(fp, 256, 256, rgb, 0);
	fclose(fp);
}

void test_rgba(void) {
	unsigned char rgba[256 * 256 * 4], *p = rgba;
	unsigned x, y;
	FILE* fp = fopen("rgba.png", "wb");
	for (y = 0; y < 256; y++)
		for (x = 0; x < 256; x++) {
			*p++ = (unsigned char)x;                /* R */
			*p++ = (unsigned char)y;                /* G */
			*p++ = 128;                             /* B */
			*p++ = (unsigned char)((x + y) / 2);    /* A */
		}
	svpng(fp, 256, 256, rgba, 1);
	fclose(fp);
}

template<class T>
T clamp(T x, T min, T max)
{
	if (x > max)
		return max;
	if (x < min)
		return min;
	return x;
}
// 输出 SRC 数组中的数据到图像
void imshow(double* SRC, int WIDTH, int HEIGHT)
{

	unsigned char* image = new unsigned char[WIDTH * HEIGHT * 3];// 图像buffer
	unsigned char* p = image;
	double* S = SRC;    // 源数据

	FILE* fp;
	fopen_s(&fp, "image.png", "wb");

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			*p++ = (unsigned char)clamp((*S++) * 255, 0.0, 255.0);  // R 通道
			*p++ = (unsigned char)clamp((*S++) * 255, 0.0, 255.0);  // G 通道
			*p++ = (unsigned char)clamp((*S++) * 255, 0.0, 255.0);  // B 通道
		}
	}

	svpng(fp, WIDTH, HEIGHT, image, 0);
}

int main(void) {
	int WIDTH = 400, HEIGHT = 200, SCREEN_Z = 0;
	double* image = new double[WIDTH * HEIGHT * 3];
	memset(image, 0.0, sizeof(double) * WIDTH * HEIGHT * 3);
	double* p = image;
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			// 像素坐标转投影平面坐标
			double x = 2.0 * double(j) / double(WIDTH) - 1.0;
			double y = 2.0 * double(HEIGHT - i) / double(HEIGHT) - 1.0;
			glm::vec3 EYE = glm::vec3(0, 0, 1);
			glm::vec3 coord = glm::vec3(x, y, SCREEN_Z);          // 计算投影平面坐标
			glm::vec3 direction = glm::normalize(coord - EYE);    // 计算光线投射方向

			glm::vec3 color = direction;

			*p = color.x; p++;  // R 通道
			*p = color.y; p++;  // G 通道
			*p = color.z; p++;  // B 通道
		}
	}

	imshow(image,WIDTH, HEIGHT);
	return 0;
}