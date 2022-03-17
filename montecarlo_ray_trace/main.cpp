#include<glm/glm.hpp>
#include<glm/glm.hpp>
#include <omp.h>    // openmp多线程加速
#include<glm/vec3.hpp>
#include<iostream>
#include"shape/Shape.h"
#include"camera/DataFrame.h"
#include"camera/Camera.h"
#include"obj/Obj.h"
#include<vector>
#include<iostream>
#include<stdio.h>

using namespace std;
using namespace glm;
const glm::vec3 RED(1, 0.5, 0.5);


DataFrame* DF;
Obj* GOBJ;
Camera* GCamera;
int main(void) {
	//DF = new DataFrame(400, 500);
	GOBJ = new Obj();
	//GOBJ->loadFile("./scenes/bedroom/bedroom.obj");
	GCamera = new Camera("bedroom");
	cout << GCamera->width << "  " << GCamera->height << endl;
	system("pause");                        
	return 0;
}