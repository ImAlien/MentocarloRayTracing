#pragma once

#include <string>

struct RenderConfig {
	std::string scenePath = "./scenes/";
	std::string sceneName = "cornell-box2";
	int spp = 1;
	int threads = 0;
	bool outlierRemoval = false;
	int outlierRadius = 1;
	double outlierThreshold = 2.5;
	bool bilateralFilter = false;
	int bilateralRadius = 2;
	double bilateralSigmaSpace = 2.0;
	double bilateralSigmaColor = 0.12;
	float stopRate = 0.6f;
	bool useMis = true;    // true: MIS integrator (light + BRDF sampling); false: legacy split diffuse/specular estimator
	bool preview = true;   // show a live GDI preview window while rendering
	int previewIntervalMs = 250;   // preview refresh period (ms); larger = less impact on render speed
	bool previewFullscreen = true; // open the preview borderless fullscreen (F11 toggles, Esc exits)
	bool progressive = true;       // true: 1spp-per-pass, image refines over time; false: one-shot full spp per pixel
	bool interactive = false;      // free-fly camera preview (WASD + mouse look); runs until the window is closed
	float moveSpeed = 1.0f;        // camera translation-speed multiplier (mouse wheel adjusts it live)
	float mouseSensitivity = 1.0f; // camera look-sensitivity multiplier
	int movingDownscale = 4;       // render at 1/N resolution while the camera is moving, full res once still
};

const RenderConfig& getRenderConfig();
void loadRenderConfig(int argc, char* argv[]);
std::string getSceneDir(const std::string& sceneName);
