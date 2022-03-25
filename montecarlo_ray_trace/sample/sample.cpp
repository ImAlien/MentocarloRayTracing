#include "sample.h"
#include "../main.h"
using namespace glm;

glm::vec3 SampleHemisphere() {
	float z = rand()*1.0/RAND_MAX;
	float r = sqrt(1.0 - z * z);
	float phi = 2.0 * PI * rand()*1.0/RAND_MAX;
	return vec3(r * cos(phi), r * sin(phi), z);
}

vec3 toNormalHemisphere(vec3 v, vec3 N) {
	vec3 helper = vec3(1, 0, 0);
	if (abs(N.x) > 0.999) helper = vec3(0, 0, 1);
	vec3 tangent = normalize(cross(N, helper));
	vec3 bitangent = normalize(cross(N, tangent));
	return v.x * tangent + v.y * bitangent + v.z * N;
}