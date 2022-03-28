
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
	vec3 I = r.direction;
	return I - 2 * dot(I, N) * N;
}
 Ray reflect(Ray&r, glm::vec3& N, glm::vec3 p) {
	 Ray res;
	 res.direction = reflect(r, N);
	 res.startPoint = p;
	 res.direction_inv = Inv(res.direction);
	 return res;
 }
