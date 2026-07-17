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
	bool preview = true;   // show a live GDI preview window while rendering
	int previewIntervalMs = 250;   // preview refresh period (ms); larger = less impact on render speed
	bool previewFullscreen = true; // open the preview borderless fullscreen (F11 toggles, Esc exits)
	bool progressive = true;       // true: 1spp-per-pass, image refines over time; false: one-shot full spp per pixel
};

const RenderConfig& getRenderConfig();
void loadRenderConfig(int argc, char* argv[]);
std::string getSceneDir(const std::string& sceneName);
