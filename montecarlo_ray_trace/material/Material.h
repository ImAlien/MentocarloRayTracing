#pragma once
#include<glm/vec3.hpp>
#include<glm/glm.hpp>
#include"../include/tiny_obj_loader.h"
#include "../main.h"
#include <iostream>
class Material {
public:
	std::string name = "";
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	double Ni, Ns;
	std::string ambient_texname;             // map_Ka
	std::string diffuse_texname;             // map_Kd
	std::string specular_texname;            // map_Ks
	std::string specular_highlight_texname;  // map_Ns
	float vtx, vty;
	Material(tinyobj::material_t);
	Material();
	bool isLight();
	void set(tinyobj::material_t);
	friend std::ostream &operator<<(std::ostream &cout,
		const Material &m) {
		cout << "Kd:  " << m.Kd.x << ' ' << m.Kd.y << ' '<< m.Kd.z << std::endl;
		return cout;
	}
};