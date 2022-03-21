
#include"scene.h"
#include"../test/Log.h"

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
	this->df = new DataFrame(camera->width, camera->height);
	shade();
	df->inputImage();
	LOG("保存图片");
	camera->saveImage(df);
}
void Scene::createBoundingBox() {
	for (Triangle* tr : obj->triangles) {
		BBs.push_back(BoundingBox(tr));
	}
}
void Scene::shade() {
	LOG("开始渲染");
	int w = camera->width, h = camera->height;
	float theta = camera->fovy / 2;
	
	float zpos = 1.0 / tan(theta);
	int idx = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			// Screen space to NDC space
			float nx = (i + 0.5f) * 2 / w - 1.0f;
			float ny = (j + 0.5f) * 2 / h - 1.0f;
			// NDC space to world space
			vec3 c_dir = vec3(nx, ny, -zpos);
			vec3 lookat_dir = normalize(camera->lookat - camera->eye);
			vec3 up_dir = normalize(camera->up);
			vec3 x_dir = normalize(cross(lookat_dir, up_dir));
			vec3 world_dir = nx * x_dir + ny * lookat_dir - zpos * up_dir;
			
			Ray ray;
			ray.direction = world_dir;
			ray.startPoint = camera->eye;
			
			vec3 color = rayCasting(ray, 0);
			df->data[idx++] = color.x;
			df->data[idx++] = color.y;
			df->data[idx++] = color.z;
		}
	}
	LOG("渲染完成");
}
vec3 Scene::rayCasting(Ray& ray, int depth) {
	if(bvh->intersectBVH(ray).isIntersect)
		return vec3(1, 0, 0);
	return vec3(0, 0, 0);
}