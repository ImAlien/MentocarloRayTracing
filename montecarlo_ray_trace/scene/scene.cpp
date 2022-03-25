
#include"scene.h"
#include"../test/Log.h"
#include <glm/gtx/vector_angle.hpp>
#include <omp.h>    // openmp多线程加速
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<map>
using namespace std;
using namespace glm;
bool flag = true;
Scene::Scene() {
}
Scene::Scene(string name) {
	camera = new Camera(name);
	string pathname = "./scenes/" + name + "/" + name + ".obj";
	obj = new Obj(pathname);
	parseFromObj();
	initMaterial();
	this->bvh = new BVH(BBs);
	this->df = new DataFrame(camera->width, camera->height);
	shade();
	df->inputImage();
	LOG("保存图片");
	camera->saveImage(df);
}
void Scene::parseFromObj() {
	for (Triangle* tr : obj->triangles) {
		BBs.push_back(BoundingBox(tr));
		if (tr->material.name == "Light") {
			Light cur(tr);
			Lights.push_back(cur);
		}
	}
	LOG("Lights number:" + to_string(Lights.size()));
}
void Scene::shade() {
	LOG("开始渲染");
	int w = camera->width, h = camera->height;
	float theta = camera->fovy/2;
	
	float zpos = 1.0 / tan(theta);

	omp_set_num_threads(10); // 线程个数
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
			
			dvec3 color = raysCasting(ray);
			gamma(color);
			int pos = (i*w + j) * 3;
			df->data[pos] = color.x;
			df->data[pos + 1] = color.y;
			df->data[pos + 2] = color.z;
		}
	}
	LOG("渲染完成");
}
dvec3 Scene::raysCasting(Ray& ray) {
	dvec3 hitColor = dvec3(0, 0, 0);
	for (int i = 0; i < SPP; i++) {
		hitColor += rayCasting(ray);
	}
	hitColor /= SPP;
	return hitColor;
}
dvec3 Scene::rayCasting(Ray& ray) {	
	dvec3 L_dir =  dvec3(0, 0, 0);
	dvec3 L_indir = dvec3(0, 0, 0);
	IntersectResult hit_res = bvh->intersectBVH(ray);
	if (hit_res.isIntersect) {
		Triangle* tr = hit_res.triangle;
		Material &m = hit_res.triangle->material;
		vec3 cur_point = hit_res.intersectPoint;
		if (m.isLight()) return vec3(17, 12, 4);
		vec3 N = tr->normal;
		vec3 L = normalize(ray.direction);
		vec3 f_r = m.BRDF(ray);
		//return m.Kd;
		//直接光照
		for (Light& l : Lights) {
			Ray ray2(cur_point, l.center);
			IntersectResult hit_res2 = bvh->intersectBVH(ray2);
			vec3 center = l.center;
			//没有被遮挡
			if (hit_res2.isIntersect) {
				if(isSamePoint(hit_res2.intersectPoint,l.center))
				L_dir += l.intensity * f_r * fabs(dot(normalize(ray2.direction), N))
				* fabs(dot(normalize(ray2.direction), l.normal))
				/ dot(l.center - cur_point, l.center - cur_point)
				/l.pdf_light;
			}
		}
		//间接光照
		float rate = rand()/(float)(RAND_MAX);
		if (rate > STOP_RATE) return L_dir + L_indir;

		Ray randomRay = randomHemisphereRay(N, cur_point);
		IntersectResult hit_res3 = bvh->intersectBVH(randomRay);
		if(hit_res3.isIntersect)
		if (!hit_res3.triangle->material.isLight()) {
			float pdf_hemi = 1.0 / (2.0 * PI);
			float cosine = fabs(dot(normalize(randomRay.direction), N));
			L_indir = f_r * cosine / pdf_hemi / STOP_RATE;
			L_indir *= rayCasting(randomRay);
		}
	}
	return L_dir + L_indir;
}
void Scene::initMaterial() {
	cv::Mat image;
	map<string, vector<Triangle*>> mapKd;
	for (Triangle* tr : obj->triangles) {
		if (tr->material.diffuse_texname != "") {
			mapKd[tr->material.diffuse_texname].push_back(tr);
		}
	}
	for (auto& m : mapKd) {
		string name = m.first;
		vector<Triangle*> trs = m.second;
		string filename = SCENE_PATH + string(SCENE_NAME) + "/" + name;
		image = cv::imread(filename);
		if (image.data == nullptr)//nullptr是c++11新出现的空指针常量
		{
			cerr << "图片文件不存在" << endl;
			exit(1);
		}
		for (Triangle* tr : trs) {
			float x = tr->tex1.x - (int)tr->tex1.x;
			float y = tr->tex1.y - (int)tr->tex1.y;
			int pic_x = image.rows * y;
			int pic_y = image.cols * x;
			//输出图片的基本信息
			int intb = image.at<cv::Vec3b>(pic_x, pic_y)[0];
			int intg = image.at<cv::Vec3b>(pic_x, pic_y)[1];
			int intr = image.at<cv::Vec3b>(pic_x, pic_y)[2];
			float b = intb * 1.0 / 255;
			float g = intg * 1.0 / 255;
			float r = intr * 1.0 / 255;
			b = powf(b, 2.2);
			g = powf(g, 2.2);
			r = powf(r, 2.2);
			tr->material.Kd = vec3(r, g ,b);
			
		}
	}
}
void gamma(dvec3& color) {
	color.x = powf(color.x, 1.0 / 2.2);
	color.y = powf(color.y, 1.0 / 2.2);
	color.z = powf(color.z, 1.0 / 2.2);
}
bool isSamePoint(vec3& a, vec3& b) {
	return fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON && fabs(a.z - b.z) < EPSILON;
}