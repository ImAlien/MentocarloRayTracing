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
#include <cmath>
#include <algorithm>

using namespace std;
using namespace glm;
const glm::vec3 RED(1, 0.5, 0.5);


DataFrame* DF;
Obj* GOBJ;
Camera* GCamera;
Scene* GScene;
std::map<std::string, glm::vec3> light_map;

static glm::vec3 rotateAround(const glm::vec3& v, float angle, const glm::vec3& axisIn) {
	glm::vec3 axis = glm::normalize(axisIn);
	float c = std::cos(angle), s = std::sin(angle);
	return v * c + glm::cross(axis, v) * s + axis * glm::dot(axis, v) * (1.0f - c);
}

static void runInteractive(PreviewWindow& window) {
	Camera* cam = GScene->camera;
	const RenderConfig& cfg = getRenderConfig();
	glm::vec3 worldUp = glm::normalize(cam->up);
	float sceneScale = glm::length(cam->lookat - cam->eye);
	if (sceneScale < 1e-3f) sceneScale = 1.0f;
	float moveSpeed = cfg.moveSpeed;
	const int step = std::max(1, cfg.movingDownscale);
	bool needFull = true;
	bool converged = false;

	GScene->resetAccum();
	LOG("interactive: WASD move, Space/Ctrl up-down, drag to look, wheel=speed, F11=fullscreen, Esc=quit");

	while (window.pump()) {
		glm::vec3 fwd = glm::normalize(cam->lookat - cam->eye);
		glm::vec3 right = glm::normalize(glm::cross(fwd, worldUp));
		bool moved = false;

		float base = sceneScale * 0.02f * moveSpeed;
		if (window.keyDown(VK_SHIFT)) base *= 4.0f;
		glm::vec3 disp(0.0f);
		if (window.keyDown('W')) disp += fwd;
		if (window.keyDown('S')) disp -= fwd;
		if (window.keyDown('D')) disp += right;
		if (window.keyDown('A')) disp -= right;
		if (window.keyDown(VK_SPACE)) disp += worldUp;
		if (window.keyDown(VK_CONTROL)) disp -= worldUp;
		if (glm::length(disp) > 1e-6f) { cam->eye += glm::normalize(disp) * base; moved = true; }

		int dx = 0, dy = 0; window.consumeMouseDelta(dx, dy);
		if (dx != 0 || dy != 0) {
			float sens = 0.0025f * cfg.mouseSensitivity;
			fwd = rotateAround(fwd, -dx * sens, worldUp);
			right = glm::normalize(glm::cross(fwd, worldUp));
			glm::vec3 nf = rotateAround(fwd, -dy * sens, right);
			if (std::fabs(glm::dot(glm::normalize(nf), worldUp)) < 0.99f) fwd = nf;
			moved = true;
		}
		int wheel = window.consumeWheel();
		if (wheel != 0) { moveSpeed *= std::pow(1.2f, (float)wheel); if (moveSpeed < 1e-3f) moveSpeed = 1e-3f; }

		if (moved) cam->lookat = cam->eye + glm::normalize(fwd);

		if (moved) {
			needFull = true;
			converged = false;
			GScene->renderCoarse(step);
			window.refresh();
			continue;
		}
		if (needFull) { GScene->resetAccum(); needFull = false; converged = false; }
		if (!converged) {
			GScene->renderPass();
			int n = GScene->sampleCount.fetch_add(1, std::memory_order_relaxed) + 1;
			GScene->resolve();
			if (n >= cfg.spp) {
				// converged to the target sample count: denoise once, then rest
				if (cfg.outlierRemoval) GScene->df->removeOutliers(cfg.outlierRadius, cfg.outlierThreshold);
				if (cfg.bilateralFilter) GScene->df->bilateralFilter(cfg.bilateralRadius, cfg.bilateralSigmaSpace, cfg.bilateralSigmaColor);
				converged = true;
			}
			window.refresh();
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	GScene->df->inputImage();
	GScene->camera->saveImage(GScene->df);
	LOG("interactive session ended, snapshot saved to image.png");
}

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
		PreviewWindow window(GScene->df, &GScene->sampleCount,
			getRenderConfig().spp,
			getRenderConfig().previewFullscreen,
			(getRenderConfig().interactive
				? std::string("MCRT [WASD/drag to fly] - ")
				: std::string("MonteCarlo Ray Trace - ")) + getRenderConfig().sceneName);
		if (window.valid() && getRenderConfig().interactive) {
			runInteractive(window);
		}
		else if (window.valid()) {
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