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
float randomf() {
	return rand()*1.0 / RAND_MAX;
}
vec3 randomCosDir() {
	float r1 = randomf(), r2 = randomf();
	float thy = 2 * PI * r1;
	float z = sqrt(r2);
	float theta= acos(sqrt(r2));
	float sintheta = sin(theta);
	return vec3(sintheta * cos(thy), sintheta * sin(thy), z);
}
vec3 randomSpeDir(int Ns) {
	float r1 = randomf(), r2 = randomf();
	float thy = 2 * PI * r1;
	float theta = acos(powf(r2, 1.0/(Ns + 1)));
	float sintheta = sin(theta);
	return vec3(sintheta * cos(thy), sintheta * sin(thy), cos(theta));
}
vec3 toWorld( vec3& N, vec3& w_dir) {
	vec3 X, Y;
	if (fabs(N.x) > fabs(N.y)) {
		float inv = 1.0f / sqrt(N.x * N.x + N.z * N.z);
		Y = vec3(N.z*inv, 0, -N.x*inv);
	}
	else {
		float inv = 1.0f / sqrt(N.y * N.y + N.z * N.z);
		Y = vec3(0, N.z*inv , -N.y*inv);
	}
	X = cross(Y, N);
	return w_dir.x * X + w_dir.y * Y + w_dir.z * N;
}
Ray randomCosWeightSampling( glm::vec3& N, glm::vec3 cur_point) {
	vec3 local = randomCosDir();
	vec3 world = toWorld(N, local);
	return Ray(cur_point, cur_point + world);
}
Ray randomSpecularSampling(glm::vec3& dir, glm::vec3 s, int Ns) {
	vec3 local = randomSpeDir(Ns);
	vec3 world = toWorld(dir, local);
	return Ray(s, s + world);
}