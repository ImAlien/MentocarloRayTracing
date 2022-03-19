
#include"scene.h"

using namespace std;
using namespace glm;

Scene::Scene() {
}
Scene::Scene(string name) {
	camera = new Camera(name);
	string pathname = "./scenes/" + name + "/" + name + ".obj";
	obj = new Obj(pathname);
	createBoundingBox();
	this->bvh = new BVH(BBs);
}
void Scene::createBoundingBox() {
	for (Triangle* tr : obj->triangles) {
		BBs.push_back(BoundingBox(tr));
	}
}