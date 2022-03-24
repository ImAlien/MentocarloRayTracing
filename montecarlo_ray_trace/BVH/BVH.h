#pragma once

#include"BoundingBox.h"
#include "../ray/IntersectResult.h"
#include "../ray/Ray.h"
#include<vector>
struct BVHnode {
	BVHnode* left, *right;
	bool isLeaf = false;
	BoundingBox BB;
	BVHnode() {};
};


class BVH {
public:
	BVHnode* root;
	BVH();
	BVH(std::vector<BoundingBox>& BBs);
	BVHnode* build(std::vector<BoundingBox>& BBs, int l, int r);
	IntersectResult intersectBVH(Ray& ray);
	IntersectResult intersectBVHnode(BVHnode* u, Ray& ray,float tmin);
};
