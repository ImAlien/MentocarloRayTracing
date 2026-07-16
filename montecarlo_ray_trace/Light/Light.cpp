#include "Light.h"
#include<iostream>
#include "../sample/sample.h"
using namespace std;
using namespace glm;

void print(vec3 p) {
	cout << "x:" <<p.x << "   y:" << p.y << "   z:" << p.z << endl;
}
Light::Light(Triangle* tr) {
	triangle = tr;
	p1 = tr->p1, p2 = tr->p2, p3 = tr->p3;
	center = p1 + p2 + p3;
	center /= 3;
	intensity = tr->getIntensity();
	float a = sqrt(dot(p2 - p1, p2 - p1));
	float b = sqrt(dot(p3 - p2, p3 - p2));
	float c = sqrt(dot(p1 - p3, p1 - p3));
	float p = (a + b + c) / 2;
	area = sqrt(p*(p - a)*(p - b)*(p - c));
	pdf_light = 1.0 / area;
	normal = tr->normal;
}
vec3 Light::randomPoint() {
	float u = randomf();
	float v = randomf();
	if (u + v > 1.0f) {
		u = 1.0f - u;
		v = 1.0f - v;
	}
	return p1 + u * (p2 - p1) + v * (p3 - p1);
}
