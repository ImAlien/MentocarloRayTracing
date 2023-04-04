
#include"Material.h"
#include "../main.h"
#include "../test/Log.h"
#include "../ray/Ray.h"
#include <tinyxml2.h>
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
	/*if(SCENE_NAME == "cornell-box")
		return name == "Light";
	else if (SCENE_NAME == "veach-mis") {
		return name.size() == 6 && name[0] == 'L';
	}
	else if (SCENE_NAME == "bedroom") {
		return name == "Light";
	}
	return false;*/
	return light_map.count(name);
}
vec3 Material::getIntensity() {
	if (!isLight()) {
		LOG("ERROR: 非光源没有亮度");
	}
	return light_map[name];
	if (SCENE_NAME == "cornell-box")
		return vec3(17,12,4);
	/*
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
	return vec3(0,0,0);*/
}
bool isSame(vec3& a, vec3& b) {
	return fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON && fabs(a.z - b.z) < EPSILON;
}
vec3 Material::BRDF(Ray& out, Ray& in, vec3& N) {
	vec3 R = normalize(reflect(out, N));
	float t1 = sqrt(dot(Ks, Ks));
	float t2 = sqrt(dot(Kd, Kd));
	if (t1 == 0 && t2 == 0) return vec3(0, 0, 0);
	float t = t1 / (t1 + t2);
	vec3 res = (1-t) * Kd/ PI + t * (Ns + 1)*Ks*powf(glm::max(0.0f,dot(normalize(in.direction), R)), Ns) / (2 * PI);
	return res;
}

float Material::fresnelRate(Ray& r,vec3 N) {
	float R0 = (Ni - 1)*(Ni - 1) / (Ni + 1) / (Ni + 1);
	if (R0 == 0) return 1;
	float cosine = fabs(dot(normalize(r.direction), N));
	return R0 + (1 - R0)* powf(1 - cosine, 5);
}

vec3 getValue(string& s) {
	int be = 0;
	vector<float> vec;
	while (be < s.size()) {
		if (isdigit(s[be])) {
			int ed = be;
			while (ed < s.size() && s[ed] != ',') ed++;
			string t = s.substr(be, ed - be);
			vec.push_back(stof(t));
			be = ed + 1;
			continue;
		}
		be++;
	}
	return { vec[0], vec[1], vec[2] };
}

void Material::init_light_map(string name) {
	string xml_path = "./scenes/" + name + "/" + name + ".xml";
	using namespace tinyxml2;
	XMLDocument doc;
	//ifstream f(xml_path.c_str());
	//cout << f.good() << endl;
	doc.LoadFile(xml_path.c_str());
	
	XMLElement* root = doc.FirstChildElement("root");
	for (XMLElement* currentele = root->FirstChildElement("light"); currentele; currentele = currentele->NextSiblingElement("light"))
	{
		XMLElement* tmpele = currentele;
		const char* name, *radiance;
		name = tmpele->Attribute("mtlname");
		radiance = tmpele->Attribute("radiance");
		string s_ra(radiance);
		vec3 vec_ra = getValue(s_ra);
		light_map[name] = vec_ra;
	}

}