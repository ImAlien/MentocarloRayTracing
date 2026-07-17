
#include"scene.h"
#include"../RenderConfig.h"
#include"../test/Log.h"
#include <glm/gtx/vector_angle.hpp>
#include <omp.h>    // openmp多线程加速
#include <stdlib.h>
#include "../sample/sample.h"
#include <time.h>
#include <set>
//#define DEBUG
#include<map>
#include <intrin.h>
#include <cstring>
using namespace std;
using namespace glm;
bool flag = true;
clock_t start_t, finish_t, pre_t;
Scene::Scene() {
}
Scene::Scene(string name) {
	this->name = name;
	const RenderConfig& config = getRenderConfig();
	start_t = clock();
	camera = new Camera(name);
	cout << camera->eye;
	cout << camera->lookat;
	Material::init_light_map(name);
	string pathname = getSceneDir(name) + name + ".obj";
	obj = new Obj(pathname);
	parseFromObj();
	LOG("加载纹理");
	initMaterial();
	
	for (auto m : light_map) {
		cout << m.first <<" " <<  m.second << endl;
	}
	LOG("纹理加载完成，共" + to_string(texmap.size()) + "个纹理");
	this->bvh = new BVH(BBs);
	this->df = new DataFrame(camera->width, camera->height);
	finish_t = clock();
	double total_t = (double)(finish_t - start_t);//将时间转换为秒
	cout << "预处理时间" << total_t << "毫秒" << endl;
}
static std::string getCpuModel() {
	int cpui[4] = { 0 };
	char brand[0x40] = { 0 };
	__cpuid(cpui, 0x80000000);
	unsigned int maxExt = (unsigned int)cpui[0];
	if (maxExt >= 0x80000004) {
		for (unsigned int id = 0x80000002; id <= 0x80000004; ++id) {
			__cpuidex(cpui, (int)id, 0);
			memcpy(brand + (id - 0x80000002) * 16, cpui, sizeof(cpui));
		}
		std::string name(brand);
		size_t end = name.find_last_not_of(" \t");
		return end == std::string::npos ? name : name.substr(0, end + 1);
	}
	return "unknown CPU";
}
void Scene::render() {
	const RenderConfig& config = getRenderConfig();
	start_t = clock();
	pre_t = clock();
	shade();
	finish_t = clock();
	double total_t = (double)(finish_t - start_t)/CLOCKS_PER_SEC;//将时间转换为秒
	cout << config.spp << "SPP渲染时间时间" << total_t << "秒" << endl;
	if (config.outlierRemoval) {
		LOG("Applying outlier removal");
		df->removeOutliers(config.outlierRadius, config.outlierThreshold);
	}
	if (config.bilateralFilter) {
		LOG("Applying bilateral filter");
		df->bilateralFilter(config.bilateralRadius, config.bilateralSigmaSpace, config.bilateralSigmaColor);
	}
	df->inputImage();
	LOG("保存图片");
	camera->saveImage(df);
	LOG("Render done | scene=" + name
		+ " | resolution=" + std::to_string(camera->width) + "x" + std::to_string(camera->height)
		+ " | spp=" + std::to_string(config.spp)
		+ " | threads=" + std::to_string(omp_get_max_threads())
		+ " | CPU=" + getCpuModel()
		+ " | render_time=" + std::to_string(total_t) + "s");
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
	progressRows.store(0);
	int w = camera->width, h = camera->height;
	// tan(fovy/2)= 0.5/zpos ,zpos指屏幕空间的z值
	float theta = camera->fovy/2;
	// 转化为弧度
	float zpos = 0.5 / tan(theta*PI/180);
	float imageRatio = w * 1.0 / h;
	const int spp = getRenderConfig().spp;
	#ifndef DEBUG
	if (getRenderConfig().threads > 0) {
		omp_set_num_threads(getRenderConfig().threads);
	}
	#pragma omp parallel for schedule(dynamic)
	#endif
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			// Screen space to NDC space
			//float ny = 1.0f - (i + 0.5f) * 2 / h ;
			//float nx = 1.0f - (j + 0.5f) * 2 / w ;
			dvec3 color = dvec3(0,0,0);
			for (int k = 0; k < spp; k++) {
					float nx,ny;
					nx = (j + randomf() - w / 2) * 1.0 / w;
					ny = (h / 2 - i + randomf()) * 1.0 / h;  
					nx *= imageRatio;
					// NDC space to world space
					vec3 c_dir = vec3(nx, ny, -zpos);
					vec3 z_dir = -normalize(camera->lookat - camera->eye);
					vec3 up_dir = normalize(camera->up);
					vec3 x_dir = normalize(cross(up_dir,z_dir));
					vec3 world_dir = nx * x_dir + ny * up_dir - zpos * z_dir;// 世界坐标系中的方向
					Ray ray;
					ray.direction = world_dir;
					ray.startPoint = camera->eye;
					ray.direction_inv = glm::vec3(1.0 / ray.direction.x, 1.0 / ray.direction.y, 1.0 / ray.direction.z);
					color += rayCasting(ray, i , j)/(1.0*spp);
			}
			gamma(color);
			int pos = (i*w + j) * 3;
			df->data[pos] = color.x;
			df->data[pos + 1] = color.y;
			df->data[pos + 2] = color.z;
		}
		progressRows.fetch_add(1, std::memory_order_relaxed);
		#ifndef DEBUG
		#pragma omp critical
		#endif
		{
			if (i % 10 == 0) {
				finish_t = clock();
				double total_t = (double)(finish_t - pre_t)/CLOCKS_PER_SEC;//将时间转换为秒
				cout  << i << "行像素时间" << total_t << "秒" << endl;
				pre_t = finish_t;
			}
		}
	}
	LOG("渲染完成");
}
dvec3 Scene::raysCasting(Ray& ray, int& x , int& y) {
	dvec3 hitColor = dvec3(0, 0, 0);
	const int spp = getRenderConfig().spp;
	for (int i = 0; i < spp; i++) {
		hitColor += rayCasting(ray, x, y);
	}
	hitColor /= spp;
	return hitColor;
}
static float vecLength(const vec3& v) {
	return sqrt(dot(v, v));
}

static float clamp01(float v) {
	if (v < 0.0f) return 0.0f;
	if (v > 1.0f) return 1.0f;
	return v;
}

static float powerHeuristic(float pdfA, float pdfB) {
	float a2 = pdfA * pdfA;
	float b2 = pdfB * pdfB;
	if (a2 + b2 <= EPSILON) return 0.0f;
	return a2 / (a2 + b2);
}

static float specularProbability(Material& m) {
	float kd = vecLength(m.Kd);
	float ks = vecLength(m.Ks);
	if (kd + ks <= EPSILON) return 0.0f;
	return clamp01(ks / (kd + ks));
}

static vec3 evalBrdf(Material& m, Ray& outRay, const vec3& wi, vec3& N) {
	float specProb = specularProbability(m);
	vec3 diffuse = (1.0f - specProb) * m.Kd / PI;
	vec3 reflectDir = normalize(reflect(outRay, N));
	float specCos = glm::max(0.0f, dot(reflectDir, normalize(wi)));
	vec3 specular = specProb * (m.Ns + 1.0f) * m.Ks * powf(specCos, m.Ns) / (2.0f * PI);
	return diffuse + specular;
}

static float brdfPdf(Material& m, Ray& outRay, const vec3& wi, vec3& N) {
	float cosTheta = glm::max(0.0f, dot(normalize(wi), N));
	float diffusePdf = cosTheta / PI;
	vec3 reflectDir = normalize(reflect(outRay, N));
	float specCos = glm::max(0.0f, dot(reflectDir, normalize(wi)));
	float specPdf = (m.Ns + 1.0f) * powf(specCos, m.Ns) / (2.0f * PI);
	float specProb = specularProbability(m);
	return (1.0f - specProb) * diffusePdf + specProb * specPdf;
}

static float lightPdfForTriangle(Scene* scene, Triangle* triangle, float dist2, float cosLight) {
	if (cosLight <= EPSILON) return 0.0f;
	for (int idx = 0; idx < (int)scene->Lights.size(); ++idx) {
		Light& light = scene->Lights[idx];
		if (light.triangle == triangle) {
			int root = scene->uflights.find(idx);
			float groupArea = scene->uflights.areas[root];
			if (groupArea <= EPSILON) return 0.0f;
			return dist2 / (cosLight * groupArea);
		}
	}
	return 0.0f;
}

static int sampleLightIndexInGroup(Scene* scene, vector<int>& lightIndices, float groupArea) {
	if (lightIndices.empty()) return -1;
	float target = randomf() * groupArea;
	float accum = 0.0f;
	for (int idx : lightIndices) {
		accum += scene->Lights[idx].area;
		if (target <= accum) return idx;
	}
	return lightIndices.back();
}

dvec3 Scene::rayCasting(Ray& ray, int& i, int& j) {
	const float stopRate = getRenderConfig().stopRate;
	dvec3 L_dir = dvec3(0, 0, 0);
	dvec3 L_indir = dvec3(0, 0, 0);
	dvec3 L_emit = dvec3(0, 0, 0);
	dvec3 L_frac = dvec3(0, 0, 0);
	double Cr = 1;
	shared_ptr<IntersectResult> hit_res = bvh->intersectBVH(ray);
	if (hit_res && hit_res->isIntersect) {
		Triangle* tr = hit_res->triangle;
		Material& m = tr->material;
		vec3 cur_point = hit_res->intersectPoint;
		m.Kd = m.isTex ? tr->getTex(hit_res, texmap[m.diffuse_texname]) : m.Kd;
		vec3 N = tr->normal;

		if (m.isLight()) {
			return m.getIntensity();
		}

		if (m.Ni > 1) {
			float rate = rand() / (float)(RAND_MAX);
			if (rate > stopRate) return L_emit;
			Ray rayFraction = refract(ray, N, cur_point, 1.0f, m.Ni);
			if (rayFraction.direction != vec3(0, 0, 0)) {
				Cr = shlick(N, normalize(ray.direction), 1.0f, m.Ni);
				L_frac = rayCasting(rayFraction, i, j) * (1.0 - Cr);
				L_frac /= stopRate;
			}
		}

		float kdLen = vecLength(m.Kd);
		float ksLen = vecLength(m.Ks);
		if (kdLen + ksLen < EPSILON) return L_frac;

		// Light sampling: sample one point on each connected light group.
		for (auto& lightgroup : divideLights) {
			int root = lightgroup.first;
			vector<int>& lightIndices = lightgroup.second;
			float groupArea = uflights.areas[root];
			int lightIndex = sampleLightIndexInGroup(this, lightIndices, groupArea);
			if (lightIndex < 0) continue;
			Light& light = Lights[lightIndex];
			vec3 lightPoint = light.randomPoint();
			vec3 toLight = lightPoint - cur_point;
			float dist2 = dot(toLight, toLight);
			if (dist2 <= EPSILON) continue;
			float dist = sqrt(dist2);
			vec3 wi = toLight / dist;
			float cosSurface = glm::max(0.0f, dot(wi, N));
			float cosLight = glm::max(0.0f, dot(-wi, light.normal));
			if (cosSurface <= EPSILON || cosLight <= EPSILON) continue;

			Ray shadowRay(cur_point + wi * EPSILON, lightPoint);
			shared_ptr<IntersectResult> shadowHit = bvh->intersectBVH(shadowRay);
			if (shadowHit && shadowHit->isIntersect && shadowHit->triangle == light.triangle) {
				float lightPdf = dist2 / (cosLight * groupArea);
				float bsdfPdf = brdfPdf(m, ray, wi, N);
				float weight = powerHeuristic(lightPdf, bsdfPdf);
				vec3 f = evalBrdf(m, ray, wi, N);
				L_dir += dvec3(light.intensity * f * cosSurface * weight / lightPdf);
			}
		}

		// BRDF sampling: cosine-weighted diffuse or Phong specular lobe.
		float rr = rand() / (float)(RAND_MAX);
		if (rr > stopRate) return L_dir * Cr + L_frac;

		float specProb = specularProbability(m);
		Ray sampleRay;
		if (randomf() < specProb) {
			vec3 reflectDir = normalize(reflect(ray, N));
			sampleRay = randomSpecularSampling(reflectDir, cur_point, m.Ns);
		}
		else {
			sampleRay = randomCosWeightSampling(N, cur_point);
		}

		vec3 wi = normalize(sampleRay.direction);
		float cosSurface = glm::max(0.0f, dot(wi, N));
		float pdf = brdfPdf(m, ray, wi, N);
		if (cosSurface > EPSILON && pdf > EPSILON) {
			sampleRay.startPoint = cur_point + wi * EPSILON;
			shared_ptr<IntersectResult> nextHit = bvh->intersectBVH(sampleRay);
			vec3 f = evalBrdf(m, ray, wi, N);
			if (nextHit && nextHit->isIntersect) {
				if (nextHit->triangle->material.isLight()) {
					vec3 toLight = nextHit->intersectPoint - cur_point;
					float dist2 = dot(toLight, toLight);
					float cosLight = glm::max(0.0f, dot(-wi, nextHit->triangle->normal));
					float lightPdf = lightPdfForTriangle(this, nextHit->triangle, dist2, cosLight);
					float weight = powerHeuristic(pdf, lightPdf);
					L_indir = dvec3(nextHit->triangle->material.getIntensity() * f * cosSurface * weight / pdf) / (double)stopRate;
				}
				else {
					L_indir = rayCasting(sampleRay, i, j) * dvec3(f * cosSurface / pdf) / (double)stopRate;
				}
			}
		}
	}
	return L_emit + (L_dir + L_indir) * Cr + L_frac;
}void Scene::initMaterial() {
	for (Triangle* tr : obj->triangles) {
		if (tr->material.diffuse_texname != "") {
			string& textureName = tr->material.diffuse_texname;
			if (texmap.count(textureName) == 0) {
				string filename = getSceneDir(name) + tr->material.diffuse_texname;
				texmap[textureName] = new Texture(filename);
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