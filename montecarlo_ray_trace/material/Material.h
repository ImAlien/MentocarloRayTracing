#pragma once
#include<glm/vec3.hpp>
#include<glm/glm.hpp>
#include"../include/tiny_obj_loader.h"
class Material {
public:
	std::string name;
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	double Ni, Ns;
	std::string ambient_texname;             // map_Ka
	std::string diffuse_texname;             // map_Kd
	std::string specular_texname;            // map_Ks
	std::string specular_highlight_texname;  // map_Ns
	Material(tinyobj::material_t);
};