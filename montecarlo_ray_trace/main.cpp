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
#include "preview/PreviewWindow.h"
#include "main.h"
#include<vector>
#include<iostream>
#include<stdio.h>
#include <time.h>
#include <thread>
#include <atomic>
#include <chrono>

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
	if (getRenderConfig().preview) {
		PreviewWindow window(GScene->df, &GScene->sampleCount, getRenderConfig().spp,
			getRenderConfig().previewFullscreen,
			"MonteCarlo Ray Trace - " + getRenderConfig().sceneName);
		if (window.valid()) {
			std::atomic<bool> done{ false };
			std::thread worker([&]() { GScene->render(); done.store(true); });

			// Live view: refresh ~30fps until the render finishes or the window is closed.
			while (!done.load()) {
				if (!window.pump()) break;   // user closed the window early
				window.refresh();
				std::this_thread::sleep_for(std::chrono::milliseconds(getRenderConfig().previewIntervalMs));
			}
			worker.join();                   // render always runs to completion and saves image.png
			window.refresh();                // show the final, post-processed frame

			// Keep the window up so the result stays visible until the user closes it.
			while (window.pump()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
			}
		}
		else {
			GScene->render();                // window creation failed; render headless
		}
	}
	else {
		GScene->render();
		system("pause");
	}
	return 0;
}