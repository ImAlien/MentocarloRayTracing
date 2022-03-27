#pragma once

#include"../camera/Camera.h"
#include"../camera/DataFrame.h"
#include"../BVH/BoundingBox.h"
#include"../BVH/BVH.h"
#include "../obj/Obj.h"
#include<vector>
#include<string>
#include"../ray/Ray.h"
#include "../main.h"
#include "../Light/Light.h"
#include "../material/Texture.h"
#include <map>
class Scene {
public:

	Camera* camera;
	std::vector<BoundingBox> BBs;
	std::vector<Light> Lights;
	Obj* obj;
	BVH* bvh;
	DataFrame* df;
	std::map<std::string, Texture*> texmap;
	Scene(std::string name);
	Scene();
	void parseFromObj();
	void shade();
	glm::dvec3 raysCasting(Ray& ray, int& i, int& j);
	glm::dvec3 rayCasting(Ray& ray, int& i, int& j);
	void initMaterial();
	void addLight(Triangle* tr);
};

void gamma(glm::dvec3& color);
bool isSamePoint(glm::vec3& a, glm::vec3& b);