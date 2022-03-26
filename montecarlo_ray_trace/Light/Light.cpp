#include "Light.h"
#include<iostream>
#include "../sample/sample.h"
using namespace std;
using namespace glm;

void print(vec3 p) {
	cout << "x:" <<p.x << "   y:" << p.y << "   z:" << p.z << endl;
}
Light::Light(Triangle* tr) {
	p1 = tr->p1, p2 = tr->p2, p3 = tr->p3;
	center = p1 + p2 + p3;
	print(p1);
	print(p2);
	print(p3);
	center /= 3;
	intensity = tr->getIntensity();
	float a = sqrt(dot(p2 - p1, p2 - p1));
	float b = sqrt(dot(p3 - p2, p3 - p2));
	float c = sqrt(dot(p1 - p3, p1 - p3));
	float p = (a + b + c) / 2;
	area = sqrt(p*(p - a)*(p - b)*(p - c));
	pdf_light = 1.0 / area;
	normal = tr->normal;
		cout << "area && pdf" << endl;
		cout << area << ' ' << pdf_light << endl;
}
vec3 Light::randomPoint() {

	float x = randomf(), y = randomf();
	vec3 t1 = p1 + 0.5f * (center - p1);
	vec3 t2 = p2 + 0.5f * (center - p2);
	vec3 t3 = p3 + 0.5f * (center - p3);
	vec3 res =  t1 * (1.0f - x) + t2 * (x *(1.0f - y)) + t3 * x*y;
	return res;
}