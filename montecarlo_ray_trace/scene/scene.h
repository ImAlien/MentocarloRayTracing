#pragma once

#include"../camera/Camera.h"
#include"../camera/DataFrame.h"
#include"../BVH/BoundingBox.h"
#include"../BVH/BVH.h"
#include "../obj/Obj.h"
#include<vector>
#include<string>
#include"../ray/Ray.h"

class Scene {
public:

	Camera* camera;
	std::vector<BoundingBox> BBs;
	Obj* obj;
	BVH* bvh;
	DataFrame* df;
	Scene(std::string name);
	Scene();
	void createBoundingBox();
	void shade();
	glm::vec3 rayCasting(Ray& ray, int depth);
};