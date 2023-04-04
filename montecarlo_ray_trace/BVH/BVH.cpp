
#include"BVH.h"
#include<glm/glm.hpp>
#include<limits.h>
#include<iostream>
#include"../test/Log.h"
using namespace std;
using namespace glm;

BVH::BVH(vector<BoundingBox>& BBs_) {
	LOG("��ʼ����BVH");
	this->BBs = BBs_;
	root = build(BBs, 0, BBs.size() - 1);
	cout << root->BB.aa.x << root->BB.bb.x; 
	LOG("BVH�������");
}

int CNT = 0;
// ��Ữ��
//BVHnode* BVH::build(vector<BoundingBox>& BBs, int l, int r) {
//	if (l > r) return nullptr;
//
//	BVHnode* node = new BVHnode();
//	if (l == r) {
//		node->isLeaf = true;
//		node->BB = BBs[l];
//		return node;
//	}
//	node->BB.aa = vec3(100, 100, 100);
//	node->BB.bb = vec3(-100, -100, -100);
//	// ���� AABB
//	for (int i = l; i <= r; i++) {
//		
//		node->BB.aa.x = glm::min(node->BB.aa.x, BBs[i].aa.x);
//		node->BB.aa.y = glm::min(node->BB.aa.y, BBs[i].aa.y);
//		node->BB.aa.z = glm::min(node->BB.aa.z, BBs[i].aa.z);
//
//		node->BB.bb.x = glm::max(node->BB.bb.x, BBs[i].bb.x);
//		node->BB.bb.y = glm::max(node->BB.bb.y, BBs[i].bb.y);
//		node->BB.bb.z = glm::max(node->BB.bb.z, BBs[i].bb.z);
//		
//	}
//
//	// �ݹ齨��
//	float lenx = node->BB.bb.x - node->BB.aa.x;
//	float leny = node->BB.bb.y - node->BB.aa.y;
//	float lenz = node->BB.bb.z - node->BB.aa.z;
//	// �� x ����
//	if (lenx >= leny && lenx >= lenz)
//		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBx);
//	// �� y ����
//	if (leny >= lenx && leny >= lenz)
//		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBy);
//	// �� z ����
//	if (lenz >= lenx && lenz >= leny)
//		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBz);
//
//	// �ݹ�
//	int mid = (l + r) / 2;
//	node->left = build(BBs, l, mid);
//	node->right = build(BBs, mid + 1, r);
//
//	return node;
//}
 
// SAH

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
	// ���� AABB
	for (int i = l; i <= r; i++) {

		node->BB.aa.x = glm::min(node->BB.aa.x, BBs[i].aa.x);
		node->BB.aa.y = glm::min(node->BB.aa.y, BBs[i].aa.y);
		node->BB.aa.z = glm::min(node->BB.aa.z, BBs[i].aa.z);

		node->BB.bb.x = glm::max(node->BB.bb.x, BBs[i].bb.x);
		node->BB.bb.y = glm::max(node->BB.bb.y, BBs[i].bb.y);
		node->BB.bb.z = glm::max(node->BB.bb.z, BBs[i].bb.z);

	}

	// �ݹ齨��
	float lenx = node->BB.bb.x - node->BB.aa.x;
	float leny = node->BB.bb.y - node->BB.aa.y;
	float lenz = node->BB.bb.z - node->BB.aa.z;
	int dim = -1;
	// �� x ����
	if (lenx >= leny && lenx >= lenz) {
		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBx);
		dim = 0;
	}
		
	// �� y ����
	if (leny >= lenx && leny >= lenz) {
		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBy);
		dim = 1;
	}
		
	// �� z ����
	if (lenz >= lenx && lenz >= leny) {
		std::sort(BBs.begin() + l, BBs.begin() + r + 1, cmpBBz);
		dim = 2;
	}
		

	// �ݹ�
	//int mid = (l + r) / 2;
	//node->left = build(BBs, l, mid);
	//node->right = build(BBs, mid + 1, r);
	int mid;
	if (r - l + 1 >= 12) {
		int ans = -1;
		const int nBuckets = 12;
		const float SAHTravCost = 0.125f;
		const int SAHInterCost = 1;
		vector<int> buckets(nBuckets, 0);
		BoundingBox bb_buckets[nBuckets];
		for (int i = l; i <= r; i++) {
			int b;
			/*switch (dim) {
				case 0: b = nBuckets * node->BB.offset(BBs[i]).x; break;
				case 1: b = nBuckets * node->BB.offset(BBs[i]).y; break;
				case 2: b = nBuckets * node->BB.offset(BBs[i]).z; break;
			}*/
			b = (i - l) * nBuckets / (r - l + 1);
			if (b == nBuckets) b = nBuckets - 1;
			buckets[b]++;   //����ÿ��bucket�����object�����Ͱ�Χ��
			bb_buckets[b].merge(BBs[i]);
		}
		vector<float> cost(nBuckets, 0);
		for (int i = 0; i < nBuckets - 1; ++i) {
			BoundingBox bounds1, bounds2;
			int count1 = 0, count2 = 0;
			for (int j = 0; j < i + 1; ++j) {
				count1 += buckets[j];
				bounds1.merge(bb_buckets[j]);
			}

			for (int j = i + 1; j < nBuckets; ++j) {
				count2 += buckets[j];
				bounds2.merge(bb_buckets[j]);
			}
			//���㻮�ֺ����ߵ�cost			
			cost[i] = SAHTravCost + (count1 * bounds1.area() + count2 * bounds2.area())
				/ node->BB.area() * SAHInterCost;
		}
		//�õ���Сcost��index		
		float minCost = cost[0];
		int minIndex = 0;
		for (int i = 0; i < nBuckets - 1; ++i) {
			if (cost[i] < minCost) {
				minIndex = i;
				minCost = cost[i];
			}
		}

		vector<int> sum(nBuckets + 1);
		for (int i = 1; i <= nBuckets; i++) sum[i] += sum[i - 1] + buckets[i - 1];

		mid = l + sum[minIndex + 1] -1;
	}
	else mid = (l + r) / 2;
	node->left = build(BBs, l, mid);
	node->right = build(BBs, mid+1, r);
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