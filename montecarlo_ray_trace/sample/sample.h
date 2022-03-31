#pragma once

#include<glm/glm.hpp>
#include<stdlib.h>
#include"../ray/Ray.h"
#include"../material/Material.h"
glm::vec3 SampleHemisphere();
glm::vec3 toNormalHemisphere(glm::vec3 v, glm::vec3 N);
Ray randomCosWeightSampling(glm::vec3& N, glm::vec3 cur_point);
float randomf();
glm::vec3 randomCosDir();
Ray randomSpecularSampling(glm::vec3& dir, glm::vec3 s, int Ns);