#include "Camera.h"
#include "../include/svpng.inc"
#include <iostream>
#include <vector>;
#include <tinyxml2.h>

#include <fstream>

using namespace std;
void Camera::saveImage(DataFrame* df) {
	FILE* fp;
	fopen_s(&fp, "image.png", "wb");
	//simpleProcess(df);
	svpng(fp, df->WIDTH, df->HEIGHT, df->image, 0);
	fclose(fp);
}
Camera::Camera() {
	defaultCamera();
}

void getValue(string& s, glm::vec3& v) {
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
	v = { vec[0], vec[1], vec[2] };
}
Camera::Camera(string scenename) {
	string xml_path = "./scenes/" + scenename + "/" + scenename + ".xml";
	using namespace tinyxml2;
	XMLDocument doc;
	ifstream f(xml_path.c_str());
	cout << f.good() << endl;
	doc.LoadFile(xml_path.c_str());
	XMLElement* camera = doc.FirstChildElement("root")->FirstChildElement("camera");
	const char* type;
	type = camera->Attribute("type");
	this->type = type;
	const char* eye = "failed";
	const char* lookat = "failed";
	const char* up = "failed";
	eye = camera->FirstChildElement("eye")->Attribute("value");
	lookat = camera->FirstChildElement("lookat")->Attribute("value");
	up = camera->FirstChildElement("up")->Attribute("value");
	string s_eye(eye), s_lookat(lookat), s_up(up);
	getValue(s_eye, this->eye);
	getValue(s_lookat, this->lookat);
	getValue(s_up, this->up);
	camera->FirstChildElement("fovy")->QueryDoubleAttribute("value", &this->fovy);
	camera->FirstChildElement("width")->QueryIntAttribute("value", &this->width);
	camera->FirstChildElement("height")->QueryIntAttribute("value", &this->height);
	/*if (scenename == "bedroom") {
		bedroomCamera();
	}
	else if (scenename == "veach-mis") {
		veachmisCamera();
	}
	else if (scenename == "cornell-box") {
		cornellboxCamera();
	}
	else {
		defaultCamera();
	}*/
}
/*
type: 类型：平行投影 or 透视投影
eye:相机位置
lookat: 相机朝向
up: 相机头顶方向
fovy: 相机观察的角度
width, heitht: 生成照片的长宽
*/
Camera::Camera(std::string type_, glm::vec3 eye_, glm::vec3 lookat_, glm::vec3 up_,
	double foxy_, int w_, int h_) {
	this->type = type_;
	this->eye = eye_;
	this->lookat = lookat_;
	this->up = up_;
	this->fovy = foxy_;
	this->width = w_;
	this->height = h_;
}
void Camera::bedroomCamera() {
	/*new (this) Camera(
		"perspective",
		glm::vec3(3.456, 1.212, 3.299),
		glm::vec3(2.699, 1.195, 2.645),
		glm::vec3(-0.013, 1.000, -0.011),
		39.4305,
		1280,
		720
	);*/
	new (this) Camera(
		"perspective",
		glm::vec3(3.456, 1.212, 3.299),
		glm::vec3(2.699, 1.195, 2.645),
		glm::vec3(-0.013, 1.000, -0.011),
		39.4305,
		1280,
		720
	);
}

void Camera::cornellboxCamera() {
	new (this) Camera(
		"perspective",
		glm::vec3(0, 1, 6.8),
		glm::vec3(0, 1, 5.8),
		glm::vec3(0, 1, 0),
		19.5,
		1024,
		1024
	);
}
void Camera::veachmisCamera() {
	new (this) Camera(
		"perspective",
		glm::vec3(0.0, 2.0, 15.0),
		glm::vec3(0.0, 1.69521, 14.0476),
		glm::vec3(0.0, 0.952421, -0.304787),
		27.3909,
		1200,
		900
	);
}
void Camera::defaultCamera() {
	new (this) Camera(
		"perspective",
		glm::vec3(0.0, 0.0, 3.0),
		glm::vec3(0.0, 0, 0),
		glm::vec3(0.0, 1, 0),
		45,
		800,
		600
	);
}
int getpos(int i, int j, int w, int c) {
	return (i * w + j)*3 + c;
}
void Camera::simpleProcess(DataFrame* df) {
	int w = df->WIDTH, h = df->HEIGHT;
	int dx[] = { -1,-1, 0, 1, 1, 1, 0, -1 , 0};
	int dy[] = { 0, -1, -1,-1, 0, 1, 1, 1 , 0};
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			for (int c = 0; c < 3; c++) {
				int ans = 0;
				int cnt = 0;
				for (int k = 0; k < 9; k++) {
					int x = dx[k] + i, y = dx[k] + j;
					if (x >= 0 && x < h && y >= 0 && y < w) {
						ans += df->image[getpos(x, y, w, c)];
						cnt++;
					}
				}
				ans /= cnt;
				df->image[getpos(i, j, w, c)] = (unsigned char)ans;
				}
			
			
		}
	}
}