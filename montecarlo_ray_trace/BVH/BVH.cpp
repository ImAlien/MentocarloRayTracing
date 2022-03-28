
#include"BVH.h"
#include<glm/glm.hpp>
#include<limits.h>
#include<iostream>
#include"../test/Log.h"
using namespace std;
using namespace glm;

BVH::BVH(vector<BoundingBox>& BBs_) {
	LOG("开始创建BVH");
	this->BBs = BBs_;
	root = build(BBs, 0, BBs.size() - 1);
	cout << root->BB.aa.x << root->BB.bb.x; 
	LOG("BVH创建完成");
}

int CNT = 0;
BVHnode* BVH::build(vector<BoundingBox>& BBs, int l, int r) {
	if (l > r) return nullptr;

	BVHnode* node = new BVHnode();
	if (l == r) {
		node->isLeaf = true;
		node->BB = BBs[l];
		return node;
	}
	node->BB.aa = vec3(100, 100, 100);
	node->BB.bb = vec3(-100, -100, -100);
	// 计算 AABB
	for (int i = l; i <= r; i++) {
		
		node->BB.aa.x = glm::min(node->BB.aa.x, BBs[i].aa.x);
		node->BB.aa.y = glm::min(node->BB.aa.y, BBs[i].aa.y);
		node->BB.aa.z = glm::min(node->BB.aa.z, BBs[i].aa.z);

		node->BB.bb.x = glm::max(node->BB.bb.x, BBs[i].bb.x);
		node->BB.bb.y = glm::max(node->BB.bb.y, BBs[i].bb.y);
		node->BB.bb.z = glm::max(node->BB.bb.z, BBs[i].bb.z);
		
	}

	// 递归建树
	float lenx = node->BB.bb.x - node->BB.aa.x;
	float leny = node->BB.bb.y - node->BB.aa.y;
	float lenz = node->BB.bb.z - node->BB.aa.z;
	// 按 x 划分
	if (lenx >= leny && lenx >= lenz)
		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBx);
	// 按 y 划分
	if (leny >= lenx && leny >= lenz)
		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBy);
	// 按 z 划分
	if (lenz >= lenx && lenz >= leny)
		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBz);

	// 递归
	int mid = (l + r) / 2;
	node->left = build(BBs, l, mid);
	node->right = build(BBs, mid + 1, r);

	return node;
}

shared_ptr<IntersectResult> BVH::intersectBVH(Ray& ray) const{
	float tmin = 0.001, tmax = INF;
	return intersectBVHnode(root, ray,tmin ,tmax);
}
shared_ptr<IntersectResult> BVH::intersectBVHnode(BVHnode* u, Ray& ray,float& tmin, float& tmax) const{
	shared_ptr<IntersectResult> res = nullptr;
	float tres = INF;
	if (u == nullptr) return res;
	
	float tcur = u->BB.intersectBB(ray);
	if (tcur == -1 || tcur >= tmax) return res;

	if (u->isLeaf) return u->BB.source->intersect(ray);
	shared_ptr<IntersectResult> res_left = intersectBVHnode(u->left, ray, tmin, tmax);
	if(res_left)tmax = std::min(tmax, res_left->distance);
	shared_ptr<IntersectResult> res_right = intersectBVHnode(u->right, ray,tmin, tmax);
	if(res_right)tmax = std::min(tmax, res_right->distance);
	if (res_left && res_left->isIntersect && res_left->distance < tres) {
		res = res_left;
		tres = res_left->distance;
	}
	if (res_right && res_right->isIntersect && res_right->distance < tres) {
		res = res_right;
		tres = res_right->distance;
	}
	/*if (res && res->isIntersect && res->triangle == nullptr) {
		LOG("ERROR : intersect but no intersectionshape");
		exit(1);
	}*/
	return res;
}