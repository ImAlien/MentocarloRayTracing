
#include "Ray.h"
#include "../main.h"
#include "../sample/sample.h"

using namespace glm;
void Ray::cutAlittle() {
	startPoint += direction * vec3(EPSILON,EPSILON, EPSILON);
}
vec3 Inv(glm::vec3 dir) {
	return vec3(1 / dir.x, 1.0 / dir.y, 1.0 / dir.z);
}
 Ray randomHemisphereRay(glm::vec3 N, glm::vec3 p) {
	Ray res;
	res.direction = toNormalHemisphere(SampleHemisphere(), N);
	res.startPoint = p;
	res.direction_inv = Inv(res.direction);
	return res;
}
 glm::vec3 reflect(Ray& r, glm::vec3& N) {
	vec3 I = normalize(r.direction);
	return I - 2 * dot(I, N) * N;
}
 Ray reflect(Ray&r, glm::vec3& N, glm::vec3& p) {
	 Ray res;
	 res.direction = reflect(r, N);
	 res.startPoint = p;
	 res.direction_inv = Inv(res.direction);
	 return res;
 }

 glm::vec3 refract(glm::vec3& N, glm::vec3& I, float n1, float n2) {
	 if (dot(N, I) > 0) {
		 N = -N;
	 }
	 float n = n1 / n2;
	 float cosI = -dot(N, I);
	 float sinT2 = n * n*(1.0 - cosI * cosI);
	 if (sinT2 > 1.0) return vec3(0, 0, 0);
	 float cosT = sqrt(1.0 - sinT2);
	 return n * I + (n*cosI - cosT)*N;

 }
 Ray refract(Ray&r, glm::vec3& N, glm::vec3&p, float n1, float n2) {
	 Ray res;
	 res.direction = refract(N, r.direction, n1, n2);
	 res.startPoint = p;
	 res.direction_inv = Inv(res.direction);
	 return res;
 }
 // fresnel approximation
 float shlick(glm::vec3 N, glm::vec3 I, float n1, float n2) {
	 float R0 = (n1 - n2) / (n1 + n2);
	 R0 *= R0;
	 float cosine = fabs(dot(N, I));
	 float x = 1.0 - cosine;
	 return R0 + (1 - R0)*x*x*x*x*x;
 }
