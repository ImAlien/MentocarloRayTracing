#include<glm/glm.hpp>
#include<glm/glm.hpp>
#include <omp.h>    // openmp多线程加速
#include<glm/vec3.hpp>
#include<iostream>
#include"shape/Shape.h"
#include"camera/DataFrame.h"
#include"camera/Camera.h"
#include"obj/Obj.h"
#include"scene/scene.h"
#include"sample/sample.h"
#include"test/Log.h"
#include "RenderConfig.h"
#include "main.h"
#include<vector>
#include<iostream>
#include<stdio.h>
#include <time.h>

using namespace std;
using namespace glm;
const glm::vec3 RED(1, 0.5, 0.5);


DataFrame* DF;
Obj* GOBJ;
Camera* GCamera;
Scene* GScene;
std::map<std::string, glm::vec3> light_map;

int main(int argc, char* argv[]) {
	//GOBJ = new Obj();
	//GOBJ->loadFile("./scenes/bunny/bunny.obj");
	loadRenderConfig(argc, argv);
	LOG("---------------------new main----------------------");
	srand(time(NULL));
	GScene = new Scene(getRenderConfig().sceneName);
	/*float sx = 0, sy = 0, sz = 0;
	int k = 1000;
	for (int i = 0; i < k; i++) {
		vec3 t = randomCosDir();
		cout << t;
		sx += t.x; sy += t.y; sz += t.z;
	}
	cout << sx/k << ' ' << sy/k << ' ' << sz/k << endl;*/
	system("pause");                        
	return 0;
}