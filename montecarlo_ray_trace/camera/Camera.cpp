#include"Camera.h"
#include "svpng.inc"


void Camera::saveImage(DataFrame* df) {
	FILE* fp;
	fopen_s(&fp, "image.png", "wb");
	svpng(fp, df->WIDTH, df->HEIGHT, df->image, 0);
	fclose(fp);
}