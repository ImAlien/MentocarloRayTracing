
#include"scene.h"
#include"../test/Log.h"
#include <glm/gtx/vector_angle.hpp>
#include <omp.h>    // openmp多线程加速

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
	float theta = camera->fovy/2;
	
	float zpos = 1.0 / tan(theta);
	int idx = 0;

	omp_set_num_threads(20); // 线程个数
	#pragma omp parallel for
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			// Screen space to NDC space
			//float ny = 1.0f - (i + 0.5f) * 2 / h ;
			//float nx = 1.0f - (j + 0.5f) * 2 / w ;
			float nx = (j - w * 1.0 / 2) / w;
			float ny = (h / 2 - i)*1.0 / h;
			// NDC space to world space
			vec3 c_dir = vec3(nx, ny, -zpos);
			vec3 lookat_dir = -normalize(camera->lookat - camera->eye);
			vec3 up_dir = normalize(camera->up);
			vec3 x_dir = normalize(cross(lookat_dir, up_dir));
			vec3 world_dir = nx * x_dir + ny * up_dir - zpos * lookat_dir;
			
			Ray ray;
			ray.direction = world_dir;
			ray.startPoint = camera->eye;
			
			dvec3 color = raysCasting(ray, 0);
			df->data[idx++] = color.x;
			df->data[idx++] = color.y;
			df->data[idx++] = color.z;
		}
	}
	LOG("渲染完成");
}
dvec3 Scene::raysCasting(Ray& ray, int depth) {
	dvec3 hitColor = dvec3(0, 0, 0);
	for (int i = 0; i < SPP; i++) {
		hitColor += rayCasting(ray, depth);
	}
	hitColor /= SPP;
	return hitColor;
}
dvec3 Scene::rayCasting(Ray& ray, int depth) {	
	dvec3 hitColor =  dvec3(0, 0, 0);
	IntersectResult hit_res = bvh->intersectBVH(ray);
	if (hit_res.isIntersect) {
		Triangle* tr = hit_res.triangle;
		Material &m = hit_res.triangle->material;
		if (m.isLight()) return vec3(17, 12, 4);
		vec3 N = tr->normal;
		vec3 L = normalize(ray.direction);
		dvec3 returnc = m.Kd * fabs(dot(N, L))*vec3(17,12,4);
		returnc /= PI;
		return returnc;
	}
	return hitColor;
}