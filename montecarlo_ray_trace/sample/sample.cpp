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
vec3 toLocal( vec3& N, vec3& w_dir) {
	vec3 help = vec3(0, 0, 1);
	vec3 x = normalize(cross(N, help));
	vec3 y = normalize(cross(N, x));
	return w_dir.x * x + w_dir.y * y + w_dir.z * N;
}
Ray randomCosWeightSampling( glm::vec3& N, glm::vec3 cur_point) {
	vec3 world_dir = randomCosDir();
	vec3 local_dir = toLocal(N, world_dir);
	return Ray(cur_point, cur_point + local_dir);
}
Ray randomSpecularSampling(glm::vec3& dir, glm::vec3 s, int Ns) {
	vec3 world_dir = randomSpeDir(Ns);
	vec3 local_dir = toLocal(dir, world_dir);
	return Ray(s, s + local_dir);
}