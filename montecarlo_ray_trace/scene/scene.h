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
#include "util.h"

class Scene {
public:

	Camera* camera;
	std::vector<BoundingBox> BBs;
	std::vector<Light> Lights;
	std::map<int, std::vector<int>> divideLights;
	UnionFind uflights;
	std::vector<float> areas;
	float lights_area = 0, lights_pdf;// in order to use one random  light stand for  multiple lights;
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
	void initufLights();
};

void gamma(glm::dvec3& color);
bool isSamePoint(glm::vec3& a, glm::vec3& b);
int binary_search(std::vector<float>& nums, float target);

