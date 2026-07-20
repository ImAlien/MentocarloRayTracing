#pragma once

// A lightweight Win32 + GDI live preview window. It reads the renderer's
// DataFrame (double RGB, gamma-corrected, [0,1]) and blits it to the window at
// whatever cadence the caller drives. No external dependencies: gdi32/user32
// are linked by MSBuild by default. Supports borderless fullscreen (F11 toggles,
// Esc exits fullscreen or closes the window).

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

#include "../camera/DataFrame.h"

class PreviewWindow {
public:
	// sampleCounter: optional pointer to the renderer's completed-pass (spp) counter,
	//   shown in the title as "spp N/total". May be null.
	// fullscreen: start in borderless fullscreen.
	PreviewWindow(DataFrame* df, std::atomic<int>* sampleCounter, int totalSamples,
		bool fullscreen, const std::string& title);
	~PreviewWindow();

	bool valid() const { return hwnd_ != nullptr; }

	// Drain pending window messages. Returns false once the window is closed.
	bool pump();

	// Copy the current framebuffer into the DIB and repaint the window.
	void refresh();

	bool closed() const { return closed_; }

	// --- input for interactive camera control ---
	bool keyDown(int vk) const { return vk >= 0 && vk < 256 && keys_[vk]; }
	// Accumulated left-drag mouse movement since the last call (resets to 0).
	void consumeMouseDelta(int& dx, int& dy) { dx = mouseDx_; dy = mouseDy_; mouseDx_ = 0; mouseDy_ = 0; }
	// Accumulated wheel notches since the last call (resets to 0).
	int consumeWheel() { int w = wheel_; wheel_ = 0; return w; }

private:
	void updatePixels();
	void paint(HDC dc);
	void setFullscreen(bool fs);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	DataFrame* df_;
	std::atomic<int>* samples_;
	int totalSamples_;
	int w_;
	int h_;
	HWND hwnd_ = nullptr;
	BITMAPINFO bmi_{};
	std::vector<uint32_t> pixels_;   // 0x00RRGGBB, top-down
	bool closed_ = false;
	bool fullscreen_ = false;
	WINDOWPLACEMENT prevPlacement_{ sizeof(WINDOWPLACEMENT) };
	DWORD savedStyle_ = 0;
	std::string baseTitle_;

	// input state
	bool keys_[256] = {};
	bool dragging_ = false;
	int mouseDx_ = 0;
	int mouseDy_ = 0;
	int lastMouseX_ = 0;
	int lastMouseY_ = 0;
	int wheel_ = 0;
};
