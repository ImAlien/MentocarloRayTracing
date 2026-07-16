#pragma once

#include <string>

struct RenderConfig {
	std::string scenePath = "./scenes/";
	std::string sceneName = "cornell-box2";
	int spp = 1;
	int threads = 0;
	bool bilateralFilter = false;
	int bilateralRadius = 2;
	double bilateralSigmaSpace = 2.0;
	double bilateralSigmaColor = 0.12;
	float stopRate = 0.6f;
};

const RenderConfig& getRenderConfig();
void loadRenderConfig(int argc, char* argv[]);
std::string getSceneDir(const std::string& sceneName);
