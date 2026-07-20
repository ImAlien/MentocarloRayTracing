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
#include <atomic>
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
	std::string name;
	Obj* obj;
	BVH* bvh;
	DataFrame* df;
	std::map<std::string, Texture*> texmap;
	std::atomic<int> progressRows{ 0 };   // completed image rows in the current pass
	std::atomic<int> sampleCount{ 0 };    // completed 1-spp passes, for the live preview title
	Scene(std::string name);
	Scene();
	void parseFromObj();
	void render();       // dispatches to progressive or one-shot, then post-process + save
	void renderPass();   // progressive: trace one sample per pixel, accumulate into df->accum
	void resolve();      // progressive: build df->data = gamma(accum / sampleCount)
	void shadeOneShot(); // one-shot: full spp per pixel, write df->data directly
	void renderCoarse(int step);  // interactive: 1 spp on a step x step grid, write df->data blocks
	void resetAccum();            // clear df->accum and sampleCount (on camera move)
	glm::dvec3 raysCasting(Ray& ray, int& i, int& j);
	glm::dvec3 rayCasting(Ray& ray, int& i, int& j);
	void initMaterial();
	void addLight(Triangle* tr);
	void initufLights();
};

void gamma(glm::dvec3& color);
bool isSamePoint(glm::vec3& a, glm::vec3& b);
int binary_search(std::vector<float>& nums, float target);

