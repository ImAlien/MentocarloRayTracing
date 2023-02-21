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
	std::vector<BoundingBox> BBs;
	BVH();
	BVH(std::vector<BoundingBox>& BBs_);
	BVHnode* build(std::vector<BoundingBox>& BBs, int l, int r);
	std::shared_ptr<IntersectResult> intersectBVH(Ray& ray) const;
	std::shared_ptr<IntersectResult> intersectBVHnode(BVHnode* u, Ray& ray,float& tmin, float& tmax) const;
};
