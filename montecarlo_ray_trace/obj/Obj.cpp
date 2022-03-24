
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
// Optional. define TINYOBJLOADER_USE_MAPBOX_EARCUT gives robust trinagulation. Requires C++11
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "Obj.h"
#include "../include/tiny_obj_loader.h"
#include "../test/Log.h"
#include <iostream>
#include<glm/glm.hpp>
using namespace std;

string getPath(string filename) {
	int pos = filename.find_last_of('/');
	return filename.substr(0, pos);
}
void Obj::loadFile(string filename) {
	string inputfile = filename;
	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = getPath(filename); // Path to material files

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(inputfile, reader_config)) {
		if (!reader.Error().empty()) {
			cerr << "TinyObjReader: " << reader.Error();
		}
		//exit(1);
	}

	if (!reader.Warning().empty()) {
		cout << "TinyObjReader: " << reader.Warning();
	}
	LOG("加载" + filename + "模型中...");
	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();
	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		int faces_nums = shapes[s].mesh.num_face_vertices.size();
		//cout << s << ' '<< faces_nums << endl;
		this->nface += faces_nums;
		for (size_t f = 0; f < faces_nums; f++) {
			Shape* cur_face = new Shape();
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				Point p;
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				double vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				double vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				double vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
				p.pos = glm::vec3(vx, vy, vz);
				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					p.normal = glm::vec3(nx, ny, nz);
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					p.tex = glm::vec2(tx, ty);
				}
				cur_face->points.push_back(p);
				// Optional: vertex colors
				// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
				// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
				// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
			}
			index_offset += fv;
			
			// per-face material
			int mtl_id = shapes[s].mesh.material_ids[f];
			if(mtl_id >= 0)
				cur_face->material.set(materials[mtl_id]);
			faces.push_back(cur_face);
		}
	}

	LOG(to_string(nface) +"个面片加载完成");
	createTriangles();
	//cout << "load success" << endl;
}

void Obj::createTriangles() {
	for (Shape* shape : faces) {
		triangles.push_back(new Triangle(shape));
	}
}
