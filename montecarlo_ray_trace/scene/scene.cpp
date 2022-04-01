
#include"scene.h"
#include"../test/Log.h"
#include <glm/gtx/vector_angle.hpp>
#include <omp.h>    // openmp多线程加速
#include <stdlib.h>
#include "../sample/sample.h"
#include <time.h>
#include <set>

#define SPP 16000
//#define DEBUG
#include<map>
using namespace std;
using namespace glm;
bool flag = true;
clock_t start_t, finish_t, pre_t;
Scene::Scene() {
}
Scene::Scene(string name) {
	start_t = clock();
	camera = new Camera(name);
	cout << camera->eye;
	cout << camera->lookat;
	string pathname = "./scenes/" + name + "/" + name + ".obj";
	obj = new Obj(pathname);
	parseFromObj();
	LOG("加载纹理");
	initMaterial();
	LOG("纹理加载完成，共" + to_string(texmap.size()) + "个纹理");
	this->bvh = new BVH(BBs);
	this->df = new DataFrame(camera->width, camera->height);
	finish_t = clock();
	double total_t = (double)(finish_t - start_t);//将时间转换为秒
	cout << "预处理时间" << total_t << "毫秒" << endl;
	start_t = clock();
	pre_t = clock();
	shade();
	finish_t = clock();
	total_t = (double)(finish_t - start_t)/CLOCKS_PER_SEC;//将时间转换为秒
	cout << SPP<< "SPP渲染时间时间" << total_t << "秒" << endl;
	df->inputImage();
	LOG("保存图片");
	camera->saveImage(df);
}
void Scene::parseFromObj() {
	for (Triangle* tr : obj->triangles) {
		BBs.push_back(BoundingBox(tr));
		addLight(tr);
	}
	float sum = 0;
	for (Light& l : Lights) {
		sum += l.area;
		areas.push_back(sum);
	}
	lights_area = sum;
	lights_pdf = 1.0 / lights_area;
	LOG("Lights number:" + to_string(Lights.size()));
	initufLights();
	set<int> roots;
	for (int i = 0; i < Lights.size(); i++) {
		roots.insert(uflights.find(i));
	}
	for (int x : roots) {
		cout << x << ' ' << uflights.areas[x] << ' ' << endl;
	}
}
void Scene::shade() {
	LOG("开始渲染");
	int w = camera->width, h = camera->height;
	float theta = camera->fovy/2;
	
	float zpos = 0.5 / tan(theta*PI/180);
	float imageRatio = w * 1.0 / h;
	for (int i = 0; i < h; i++) {
		#ifndef DEBUG
		omp_set_num_threads(20); // 线程个数
		#pragma omp parallel for
		#endif
		for (int j = 0; j < w; j++) {
			// Screen space to NDC space
			//float ny = 1.0f - (i + 0.5f) * 2 / h ;
			//float nx = 1.0f - (j + 0.5f) * 2 / w ;
			dvec3 color = dvec3(0,0,0);
			for (int k = 0; k < SPP; k++) {
					float nx,ny;
					nx = (j + randomf() - w / 2) * 1.0 / w;
					ny = (h / 2 - i + randomf()) * 1.0 / h;
					nx *= imageRatio;
					// NDC space to world space
					vec3 c_dir = vec3(nx, ny, -zpos);
					vec3 z_dir = -normalize(camera->lookat - camera->eye);
					vec3 up_dir = normalize(camera->up);
					vec3 x_dir = normalize(cross(up_dir,z_dir));
					vec3 world_dir = nx * x_dir + ny * up_dir - zpos * z_dir;
					Ray ray;
					ray.direction = world_dir;
					ray.startPoint = camera->eye;
					ray.direction_inv = glm::vec3(1.0 / ray.direction.x, 1.0 / ray.direction.y, 1.0 / ray.direction.z);
					color += rayCasting(ray, i , j)/(1.0*SPP);
			}
			gamma(color);
			int pos = (i*w + j) * 3;
			df->data[pos] = color.x;
			df->data[pos + 1] = color.y;
			df->data[pos + 2] = color.z;
		}
		if (i % 10 == 0) {
			finish_t = clock();
			double total_t = (double)(finish_t - pre_t)/CLOCKS_PER_SEC;//将时间转换为秒
			cout  << i << "行像素时间" << total_t << "秒" << endl;
			pre_t = finish_t;
		}
	}
	LOG("渲染完成");
}
dvec3 Scene::raysCasting(Ray& ray, int& x , int& y) {
	dvec3 hitColor = dvec3(0, 0, 0);
	for (int i = 0; i < SPP; i++) {
		hitColor += rayCasting(ray, x, y);
	}
	hitColor /= SPP;
	return hitColor;
}
dvec3 Scene::rayCasting(Ray& ray, int& i, int& j) {	
	dvec3 L_dir =  dvec3(0, 0, 0);
	dvec3 L_indir = dvec3(0, 0, 0);
	dvec3 L_emit = dvec3(0, 0, 0);
	shared_ptr<IntersectResult> hit_res = bvh->intersectBVH(ray);
	//return dvec3(1, 0, 0);
	if (hit_res && hit_res->isIntersect) {
		Triangle* tr = hit_res->triangle;
		Material &m = hit_res->triangle->material;
		vec3 cur_point = hit_res->intersectPoint;
		m.Kd = m.isTex ? tr->getTex(hit_res, texmap[m.diffuse_texname]) : m.Kd;
		vec3 N = tr->normal;
		vec3 L = normalize(ray.direction);
		//是否光源
		if (m.isLight())  L_emit = m.getIntensity();
		//return m.Kd;

		float t1 = sqrt(dot(m.Ks, m.Ks));
		float t2 = sqrt(dot(m.Kd, m.Kd));
		if (t1 + t2 == 0) return L_emit + L_dir;
		float t = t1 / (t1 + t2);
		float rate2 = randomf();
		if (rate2 >= t) {
			//直接光照
			float random_area = randomf() * lights_area;
			for (auto& lightgroup : divideLights) {
				int x = lightgroup.first;
				vector<int>& p_Lights = lightgroup.second;
				int k = randomf() * (p_Lights.size() - 1);
				float t_area = uflights.areas[x];
				Light& l = Lights[p_Lights[k]];
				vec3 center = l.randomPoint();
				//vec3 center = l.center;
				Ray ray2(cur_point, center);
				shared_ptr<IntersectResult> hit_res2;
				if (dot(ray2.direction, l.normal) < 0) {
					hit_res2 = bvh->intersectBVH(ray2);
					//没有被遮挡
					if (hit_res2 && hit_res2->isIntersect) {
						vec3 f_r = m.BRDF(ray, ray2, N);
						if (isSamePoint(hit_res2->intersectPoint, center))
							//if (hit_res2->triangle->material.isLight())
							L_dir += l.intensity * f_r * fabs(dot(normalize(ray2.direction), N))
							* fabs(dot(normalize(ray2.direction), l.normal))
							/ dot(center - cur_point, center - cur_point)
							* t_area;
					}
				}
			}
			//间接光照
			float rate = rand() / (float)(RAND_MAX);
			if (rate > STOP_RATE) return L_dir + L_indir + L_emit;
			vec3 reflect_ray = reflect(ray, N);
			Ray cosRandomRay = randomCosWeightSampling(N, cur_point);
			shared_ptr<IntersectResult> hit_res3 = bvh->intersectBVH(cosRandomRay);
			if (hit_res3 && hit_res3->isIntersect) {
				bool isLight = hit_res3->triangle->material.isLight();
				if (!isLight) {
					//float pdf = dot(N, cosRandomRay.direction)/PI;
						//L_indir = m.BRDF(ray, cosRandomRay, N)/pdf / STOP_RATE/(1-t);
						//L_indir *= rayCasting(cosRandomRay, i, j);
					vec3 f_r = m.Kd / dot(N, cosRandomRay.direction);
					L_indir = f_r / STOP_RATE;
					L_indir *= rayCasting(cosRandomRay, i, j);
				}
			//	}
			}
		}
		else {
			//间接光照
			float rate = rand() / (float)(RAND_MAX);
			if (rate > STOP_RATE) return L_dir + L_indir + L_emit;

			vec3 reflect_ray = reflect(ray, N);
			Ray specularRay = randomSpecularSampling(reflect_ray, cur_point, m.Ns);
			shared_ptr<IntersectResult> hit_res3 = bvh->intersectBVH(specularRay);
			if (hit_res3 && hit_res3->isIntersect) {
				bool isLight = hit_res3->triangle->material.isLight();
				//if (!isLight) {
					//float pdf = (m.Ns + 1)/(2*PI)*powf(dot(reflect_ray,specularRay.direction), m.Ns);
					//L_indir = m.BRDF(ray, specularRay, N) / pdf/STOP_RATE/t;
				vec3 f_r = m.Ks*(m.Ns + 2) / (m.Ns + 1);
				L_indir = f_r / STOP_RATE;
				L_indir *= rayCasting(specularRay, i, j);
				//if (isLight) L_indir *= RAND2;
				//}
			}
		}
		//Ray randomRay = randomHemisphereRay(N, cur_point);
		//shared_ptr<IntersectResult> hit_res3 = bvh->intersectBVH(randomRay);
		//if (hit_res3 && hit_res3->isIntersect) {
		//	bool isLight = hit_res3->triangle->material.isLight();
		//	//if (!isLight || isLight && randomf() > RAND2) {
		//		float pdf_hemi = 1.0 / (2.0 * PI);
		//		float cosine = fabs(dot(normalize(randomRay.direction), N));
		//		L_indir = m.BRDF(ray, randomRay, N) * cosine / pdf_hemi / STOP_RATE;
		//		L_indir *= rayCasting(randomRay, i, j);
		//		//if (isLight) L_indir *= RAND2;
		//	//}
		//}
		
	}
	return L_emit + L_dir + L_indir;
}
void Scene::initMaterial() {
	for (Triangle* tr : obj->triangles) {
		if (tr->material.diffuse_texname != "") {
			string& name = tr->material.diffuse_texname;
			if (texmap.count(name) == 0) {
				string filename = SCENE_PATH + string(SCENE_NAME) + "/" + tr->material.diffuse_texname;
				texmap[name] = new Texture(filename);
			}
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

void Scene::addLight(Triangle* tr) {
	if (tr->material.name.substr(0,5) == "Light") {
		Lights.push_back(Light(tr));
	}
}

int binary_search(vector<float>& nums, float target) {
	int l = 0, r = nums.size() - 1;
	while (l < r) {
		int mid = (l + r) / 2;
		if (nums[mid] > target) r = mid;
		else l = mid + 1;
	}
	return l;
}
bool connect(vector<Light>& lights, int i, int j) {
	Light& x = lights[i], &y = lights[j];
	if (x.p1 == y.p1 || x.p2 == y.p1 || x.p3 == y.p1) return true;
	if (x.p1 == y.p2 || x.p2 == y.p2 || x.p3 == y.p2) return true;
	if (x.p1 == y.p2 || x.p2 == y.p2 || x.p3 == y.p2) return true;
	return false;
}
void Scene::initufLights() {
	int n = Lights.size();
	uflights.init(n);
	for (int i = 0; i < n; i++) {
		uflights.areas[i] = Lights[i].area;
	}
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (connect(Lights, i, j)) {
				uflights.merge(i, j);
			}
		}
	}
	for (int i = 0; i < n; i++) divideLights[uflights.find(i)].push_back(i);
	
	for (auto m : divideLights) {
		cout <<"光源:" <<m.first << ' ' << m.second.size() << endl;
	}
}