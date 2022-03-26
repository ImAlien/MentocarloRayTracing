#pragma once

#include<glm/glm.hpp>
#include<stdlib.h>
glm::vec3 SampleHemisphere();
glm::vec3 toNormalHemisphere(glm::vec3 v, glm::vec3 N);
float randomf();