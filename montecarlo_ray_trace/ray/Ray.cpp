
#include "Ray.h"
#include "../main.h"
#include "../sample/sample.h"

using namespace glm;
void Ray::cutAlittle() {
	startPoint += direction * vec3(EPSILON,EPSILON, EPSILON);
}
Ray randomHemisphereRay(glm::vec3 N, glm::vec3 p) {
	Ray res;
	res.direction = toNormalHemisphere(SampleHemisphere(), N);
	res.startPoint = p;
	return res;
}

