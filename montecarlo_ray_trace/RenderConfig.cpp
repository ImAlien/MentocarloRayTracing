#include "RenderConfig.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {
RenderConfig g_config;
std::string g_configBaseDir;

std::string trim(const std::string& value) {
	const char* whitespace = " \t\r\n";
	const size_t begin = value.find_first_not_of(whitespace);
	if (begin == std::string::npos) {
		return "";
	}
	const size_t end = value.find_last_not_of(whitespace);
	return value.substr(begin, end - begin + 1);
}

void normalizeScenePath() {
	if (!g_config.scenePath.empty()) {
		const char last = g_config.scenePath[g_config.scenePath.size() - 1];
		if (last != '/' && last != '\\') {
			g_config.scenePath += "/";
		}
	}
}

bool isAbsolutePath(const std::string& path) {
	return (path.size() > 1 && path[1] == ':')
		|| (!path.empty() && (path[0] == '/' || path[0] == '\\'));
}

std::string getDirectoryName(const std::string& filename) {
	const size_t pos = filename.find_last_of("/\\");
	if (pos == std::string::npos) {
		return "";
	}
	return filename.substr(0, pos + 1);
}

void applyOption(const std::string& key, const std::string& value) {
	if (key == "scene") {
		g_config.sceneName = value;
	}
	else if (key == "scene_path") {
		g_config.scenePath = !g_configBaseDir.empty() && !isAbsolutePath(value)
			? g_configBaseDir + value
			: value;
		normalizeScenePath();
	}
	else if (key == "spp") {
		g_config.spp = std::max(1, std::atoi(value.c_str()));
	}
	else if (key == "threads") {
		g_config.threads = std::max(0, std::atoi(value.c_str()));
	}
	else if (key == "outlier_removal") {
		g_config.outlierRemoval = value == "1" || value == "true" || value == "on";
	}
	else if (key == "outlier_radius") {
		g_config.outlierRadius = std::max(1, std::atoi(value.c_str()));
	}
	else if (key == "outlier_threshold") {
		g_config.outlierThreshold = std::max(1.001, std::atof(value.c_str()));
	}
	else if (key == "bilateral_filter") {
		g_config.bilateralFilter = value == "1" || value == "true" || value == "on";
	}
	else if (key == "bilateral_radius") {
		g_config.bilateralRadius = std::max(1, std::atoi(value.c_str()));
	}
	else if (key == "bilateral_sigma_space") {
		g_config.bilateralSigmaSpace = std::max(0.001, std::atof(value.c_str()));
	}
	else if (key == "bilateral_sigma_color") {
		g_config.bilateralSigmaColor = std::max(0.001, std::atof(value.c_str()));
	}
	else if (key == "stop_rate") {
		g_config.stopRate = static_cast<float>(std::atof(value.c_str()));
		if (g_config.stopRate <= 0.0f || g_config.stopRate > 1.0f) {
			g_config.stopRate = 0.6f;
		}
	}
	else if (key == "preview") {
		g_config.preview = value == "1" || value == "true" || value == "on";
	}
	else if (key == "preview_interval_ms") {
		g_config.previewIntervalMs = std::max(16, std::atoi(value.c_str()));
	}
	else if (key == "preview_fullscreen") {
		g_config.previewFullscreen = value == "1" || value == "true" || value == "on";
	}
	else if (key == "progressive") {
		g_config.progressive = value == "1" || value == "true" || value == "on";
	}
}

bool loadConfigFile(const std::string& filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		return false;
	}

	g_configBaseDir = getDirectoryName(filename);

	std::string line;
	while (std::getline(file, line)) {
		const size_t comment = line.find('#');
		if (comment != std::string::npos) {
			line = line.substr(0, comment);
		}

		const size_t eq = line.find('=');
		if (eq == std::string::npos) {
			continue;
		}

		applyOption(trim(line.substr(0, eq)), trim(line.substr(eq + 1)));
	}
	return true;
}
}

const RenderConfig& getRenderConfig() {
	return g_config;
}

void loadRenderConfig(int argc, char* argv[]) {
	std::string configFile = "render_config.txt";

	for (int i = 1; i < argc; ++i) {
		const std::string arg = argv[i];
		if (arg == "--config" && i + 1 < argc) {
			configFile = argv[++i];
		}
	}

	if (!loadConfigFile(configFile) && configFile == "render_config.txt") {
		if (!loadConfigFile("montecarlo_ray_trace/render_config.txt")) {
			loadConfigFile("../../montecarlo_ray_trace/render_config.txt");
		}
	}

	g_configBaseDir = "";

	for (int i = 1; i < argc; ++i) {
		const std::string arg = argv[i];
		if (arg == "--config" && i + 1 < argc) {
			++i;
		}
		else if (arg == "--scene" && i + 1 < argc) {
			applyOption("scene", argv[++i]);
		}
		else if (arg == "--scene-path" && i + 1 < argc) {
			applyOption("scene_path", argv[++i]);
		}
		else if (arg == "--spp" && i + 1 < argc) {
			applyOption("spp", argv[++i]);
		}
		else if (arg == "--threads" && i + 1 < argc) {
			applyOption("threads", argv[++i]);
		}
		else if (arg == "--outlier-removal" && i + 1 < argc) {
			applyOption("outlier_removal", argv[++i]);
		}
		else if (arg == "--outlier-radius" && i + 1 < argc) {
			applyOption("outlier_radius", argv[++i]);
		}
		else if (arg == "--outlier-threshold" && i + 1 < argc) {
			applyOption("outlier_threshold", argv[++i]);
		}
		else if (arg == "--bilateral-filter" && i + 1 < argc) {
			applyOption("bilateral_filter", argv[++i]);
		}
		else if (arg == "--bilateral-radius" && i + 1 < argc) {
			applyOption("bilateral_radius", argv[++i]);
		}
		else if (arg == "--bilateral-sigma-space" && i + 1 < argc) {
			applyOption("bilateral_sigma_space", argv[++i]);
		}
		else if (arg == "--bilateral-sigma-color" && i + 1 < argc) {
			applyOption("bilateral_sigma_color", argv[++i]);
		}
		else if (arg == "--stop-rate" && i + 1 < argc) {
			applyOption("stop_rate", argv[++i]);
		}
		else if (arg == "--preview" && i + 1 < argc) {
			applyOption("preview", argv[++i]);
		}
		else if (arg == "--preview-interval" && i + 1 < argc) {
			applyOption("preview_interval_ms", argv[++i]);
		}
		else if (arg == "--preview-fullscreen" && i + 1 < argc) {
			applyOption("preview_fullscreen", argv[++i]);
		}
		else if (arg == "--progressive" && i + 1 < argc) {
			applyOption("progressive", argv[++i]);
		}
		else if (!arg.empty() && arg[0] != '-') {
			applyOption("scene", arg);
		}
	}

	normalizeScenePath();
	std::cout << "Scene: " << g_config.sceneName
		<< ", SPP: " << g_config.spp
		<< ", Threads: " << (g_config.threads > 0 ? std::to_string(g_config.threads) : "auto")
		<< ", Outlier removal: " << (g_config.outlierRemoval ? "on" : "off")
		<< ", Bilateral: " << (g_config.bilateralFilter ? "on" : "off")
		<< ", Scene path: " << g_config.scenePath
		<< ", Stop rate: " << g_config.stopRate
		<< ", Preview: " << (g_config.preview ? "on" : "off") << std::endl;
}

std::string getSceneDir(const std::string& sceneName) {
	return getRenderConfig().scenePath + sceneName + "/";
}
