#include"util.h"
#include "../main.h"
using namespace std;
using namespace glm;
bool checkNan(vec3 v) {
	return isnan(v.x) || isnan(v.y) || isnan(v.z);
}