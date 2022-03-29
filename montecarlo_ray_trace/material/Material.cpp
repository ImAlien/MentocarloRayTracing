
#include"Material.h"
#include "../main.h"
#include "../test/Log.h"
#include "../ray/Ray.h"

using namespace glm;
using namespace std;

Material::Material(tinyobj::material_t mt) {
	this->Ka = glm::vec3(mt.ambient[0], mt.ambient[1], mt.ambient[2]);
	this->Kd = glm::vec3(mt.diffuse[0], mt.diffuse[1], mt.diffuse[2]);
	this->Ks = glm::vec3(mt.specular[0], mt.specular[1], mt.specular[2]);
	this->Ni = mt.ior;
	this->name = mt.name;
	this->ambient_texname = mt.ambient_texname;
	this->diffuse_texname = mt.diffuse_texname;
	if (diffuse_texname != "") isTex = true;
	this->specular_texname = mt.specular_texname;
	this->specular_highlight_texname = mt.specular_highlight_texname;
	this->Ns = mt.shininess;
}
void Material::set(tinyobj::material_t mt) {
	this->Ka = glm::vec3(mt.ambient[0], mt.ambient[1], mt.ambient[2]);
	this->Kd = glm::vec3(mt.diffuse[0], mt.diffuse[1], mt.diffuse[2]);
	this->Ks = glm::vec3(mt.specular[0], mt.specular[1], mt.specular[2]);
	this->Ni = mt.ior;
	this->name = mt.name;
	this->ambient_texname = mt.ambient_texname;
	this->diffuse_texname = mt.diffuse_texname;
	if (diffuse_texname != "") isTex = true;
	this->specular_texname = mt.specular_texname;
	this->specular_highlight_texname = mt.specular_highlight_texname;
	this->Ns = mt.shininess;
}

Material::Material() {
	this->Ka = glm::vec3(0, 0, 0);
}
bool Material::isLight() {
	if(SCENE_NAME == "cornell-box")
		return name == "Light";
	else if (SCENE_NAME == "veach-mis") {
		return name.size() == 6 && name[0] == 'L';
	}
	else if (SCENE_NAME == "bedroom") {
		return name == "Light";
	}
	return false;
}
vec3 Material::getIntensity() {
	if (!isLight()) {
		LOG("ERROR: 非光源没有亮度");
	}
	if (SCENE_NAME == "cornell-box")
		return vec3(17,12,4);
	else if (SCENE_NAME == "veach-mis") {
		if (name[5] == '1') return vec3(901.803, 901.803, 901.803);
		if (name[5] == '2') return vec3(100.0, 100.0, 100.0);
		if (name[5] == '3') return vec3(11.1111, 11.1111, 11.1111);
		if (name[5] == '4') return vec3(1.23457, 1.23457, 1.23457);
		if (name[5] == '5') return vec3(800.0, 800.0, 800.0);
	}
	else if (SCENE_NAME == "bedroom") {
		return vec3(16.4648, 16.4648, 16.4648);
	}
	return vec3(0,0,0);
}
bool isSame(vec3& a, vec3& b) {
	return fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON && fabs(a.z - b.z) < EPSILON;
}
vec3 Material::BRDF(Ray& out, Ray& in, vec3& N) {
	vec3 res = Kd / PI;
	vec3 R = normalize(reflect(in, N));
	res += (Ns + 2)*Ks*powf(fabs(dot(normalize(out.direction), R)), Ns) / (2 * PI);
	return res;
}

float Material::fresnelRate(Ray& r,vec3 N) {
	float R0 = (Ni - 1)*(Ni - 1) / (Ni + 1) / (Ni + 1);
	if (R0 == 0) return 1;
	float cosine = fabs(dot(normalize(r.direction), N));
	return R0 + (1 - R0)* powf(1 - cosine, 5);
}