
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace glm;

vec3 Texture::get(float x, float y) {
	if (x > 1 || y > 1) {
		cerr <<"纹理坐标大于1";
	}
	if (x < 0 || y < 0) {
		cerr << "纹理坐标小于0";
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
		cerr << "读取纹理失败" << endl;
		cout <<' ' << i << ' ' <<j << endl;
	}
	return res;
}
Texture::Texture(string filename) {
	int channels = 0;
	unsigned char* image = stbi_load(filename.c_str(), &w, &h, &channels, STBI_rgb);
	if (image == nullptr) {
		cerr << "Failed to load image: " << filename << " (" << stbi_failure_reason() << ")" << endl;
		exit(1);
	}
	tex = new vec3[w*h];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			const int offset = (i*w + j) * STBI_rgb;
			float r = powf(image[offset] / 255.0f, 2.2f);
			float g = powf(image[offset + 1] / 255.0f, 2.2f);
			float b = powf(image[offset + 2] / 255.0f, 2.2f);
			tex[i*w +j] = vec3(r, g, b);
		}
	}
	stbi_image_free(image);
}
