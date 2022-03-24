
#include"Material.h"

using namespace glm;

Material::Material(tinyobj::material_t mt) {
	this->Ka = glm::vec3(mt.ambient[0], mt.ambient[1], mt.ambient[2]);
	this->Kd = glm::vec3(mt.diffuse[0], mt.diffuse[1], mt.diffuse[2]);
	this->Ks = glm::vec3(mt.specular[0], mt.specular[1], mt.specular[2]);
	this->name = mt.name;
	this->ambient_texname = mt.ambient_texname;
	this->diffuse_texname = mt.diffuse_texname;
	this->specular_texname = mt.specular_texname;
	this->specular_highlight_texname = mt.specular_highlight_texname;
	this->Ns = mt.shininess;
}
void Material::set(tinyobj::material_t mt) {
	this->Ka = glm::vec3(mt.ambient[0], mt.ambient[1], mt.ambient[2]);
	this->Kd = glm::vec3(mt.diffuse[0], mt.diffuse[1], mt.diffuse[2]);
	this->Ks = glm::vec3(mt.specular[0], mt.specular[1], mt.specular[2]);
	this->name = mt.name;
	this->ambient_texname = mt.ambient_texname;
	this->diffuse_texname = mt.diffuse_texname;
	this->specular_texname = mt.specular_texname;
	this->specular_highlight_texname = mt.specular_highlight_texname;
	this->Ns = mt.shininess;
}

Material::Material() {
	this->Ka = glm::vec3(0, 0, 0);
}
bool Material::isLight() {
	return name == "Light";
}