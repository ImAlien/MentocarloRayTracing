#pragma once

// A lightweight Win32 + GDI live preview window. It reads the renderer's
// DataFrame (double RGB, gamma-corrected, [0,1]) and blits it to a resizable
// window at whatever cadence the caller drives. No external dependencies:
// gdi32/user32 are linked by MSBuild by default.

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
	// progress: optional pointer to the renderer's completed-row counter, used
	// to show a percentage in the title bar. May be null.
	PreviewWindow(DataFrame* df, std::atomic<int>* progress, const std::string& title);
	~PreviewWindow();

	bool valid() const { return hwnd_ != nullptr; }

	// Drain pending window messages. Returns false once the window is closed.
	bool pump();

	// Copy the current framebuffer into the DIB and repaint the window.
	void refresh();

	bool closed() const { return closed_; }

private:
	void updatePixels();
	void paint(HDC dc);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	DataFrame* df_;
	std::atomic<int>* progress_;
	int w_;
	int h_;
	HWND hwnd_ = nullptr;
	BITMAPINFO bmi_{};
	std::vector<uint32_t> pixels_;   // 0x00RRGGBB, top-down
	bool closed_ = false;
	std::string baseTitle_;
};
