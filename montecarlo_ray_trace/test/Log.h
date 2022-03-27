#pragma once
#include <string>
#include <iostream>
#include<glm/glm.hpp>
void LOG(std::string strMsg);
std::ostream &operator<<(std::ostream& os, const glm::vec3& v);
