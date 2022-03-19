#pragma once

#include"../camera/Camera.h"
#include"../BVH/BoundingBox.h"
#include"../BVH/BVH.h"
#include "../obj/Obj.h"
#include<vector>
#include<string>

class Scene {
public:

	Camera* camera;
	std::vector<BoundingBox> BBs;
	Obj* obj;
	BVH* bvh;
	Scene(std::string name);
	Scene();
	void createBoundingBox();
};