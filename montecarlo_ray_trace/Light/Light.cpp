#include "Light.h"
#include<iostream>
using namespace std;
using namespace glm;

Light::Light(Triangle* tr) {
	vec3 p1 = tr->p1, p2 = tr->p2, p3 = tr->p3;
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
		cout << "area && pdf" << endl;
		cout << area << ' ' << pdf_light << endl;
}