#include "PreviewWindow.h"

#include <windowsx.h>   // GET_X_LPARAM / GET_Y_LPARAM
#include <algorithm>

namespace {
const char* kClassName = "MCRT_PreviewWindow";
const int kSceneMenuBase = 1000;   // WM_COMMAND ids for the scene popup menu

inline uint32_t toByte(double v) {
	int x = static_cast<int>(v * 255.0 + 0.5);
	if (x < 0) x = 0;
	if (x > 255) x = 255;
	return static_cast<uint32_t>(x);
}
}

PreviewWindow::PreviewWindow(DataFrame* df, std::atomic<int>* sampleCounter, int totalSamples,
	bool fullscreen, const std::string& title)
	: df_(df), samples_(sampleCounter), totalSamples_(totalSamples),
	  w_(df ? df->WIDTH : 0), h_(df ? df->HEIGHT : 0), baseTitle_(title) {
	if (!df_ || w_ <= 0 || h_ <= 0) {
		return;
	}

	HINSTANCE hInst = GetModuleHandle(nullptr);
	WNDCLASSA wc{};
	wc.lpfnWndProc = &PreviewWindow::WndProc;
	wc.hInstance = hInst;
	wc.lpszClassName = kClassName;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	RegisterClassA(&wc);   // harmless if the class already exists

	// Start with a client area matching the image, clamped to the screen.
	const int scrW = GetSystemMetrics(SM_CXSCREEN);
	const int scrH = GetSystemMetrics(SM_CYSCREEN);
	const int cw = std::min(w_, std::max(320, scrW - 120));
	const int ch = std::min(h_, std::max(240, scrH - 160));
	RECT r{ 0, 0, cw, ch };
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

	hwnd_ = CreateWindowExA(0, kClassName, baseTitle_.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
		nullptr, nullptr, hInst, this);
	if (!hwnd_) {
		return;
	}

	bmi_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi_.bmiHeader.biWidth = w_;
	bmi_.bmiHeader.biHeight = -h_;   // negative => top-down rows
	bmi_.bmiHeader.biPlanes = 1;
	bmi_.bmiHeader.biBitCount = 32;
	bmi_.bmiHeader.biCompression = BI_RGB;

	pixels_.assign(static_cast<size_t>(w_) * h_, 0u);

	ShowWindow(hwnd_, SW_SHOW);
	UpdateWindow(hwnd_);
	SetForegroundWindow(hwnd_);
	SetFocus(hwnd_);   // so WASD / mouse reach the window in interactive mode

	if (fullscreen) {
		setFullscreen(true);
	}
}

PreviewWindow::~PreviewWindow() {
	if (hwnd_) {
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
	}
}

void PreviewWindow::setFullscreen(bool fs) {
	if (!hwnd_ || fs == fullscreen_) return;
	fullscreen_ = fs;
	if (fs) {
		savedStyle_ = GetWindowLong(hwnd_, GWL_STYLE);
		GetWindowPlacement(hwnd_, &prevPlacement_);
		MONITORINFO mi{ sizeof(MONITORINFO) };
		GetMonitorInfo(MonitorFromWindow(hwnd_, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowLong(hwnd_, GWL_STYLE, savedStyle_ & ~static_cast<LONG>(WS_OVERLAPPEDWINDOW));
		SetWindowPos(hwnd_, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	else {
		SetWindowLong(hwnd_, GWL_STYLE, savedStyle_ ? savedStyle_ : WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd_, &prevPlacement_);
		SetWindowPos(hwnd_, nullptr, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	InvalidateRect(hwnd_, nullptr, TRUE);
}

void PreviewWindow::setSource(DataFrame* df) {
	if (!df) return;
	df_ = df;
	w_ = df->WIDTH;
	h_ = df->HEIGHT;
	bmi_.bmiHeader.biWidth = w_;
	bmi_.bmiHeader.biHeight = -h_;
	pixels_.assign(static_cast<size_t>(w_) * h_, 0u);
	if (hwnd_ && !fullscreen_) {
		const int scrW = GetSystemMetrics(SM_CXSCREEN);
		const int scrH = GetSystemMetrics(SM_CYSCREEN);
		const int cw = std::min(w_, std::max(320, scrW - 120));
		const int ch = std::min(h_, std::max(240, scrH - 160));
		RECT r{ 0, 0, cw, ch };
		AdjustWindowRect(&r, static_cast<DWORD>(GetWindowLong(hwnd_, GWL_STYLE)), FALSE);
		SetWindowPos(hwnd_, nullptr, 0, 0, r.right - r.left, r.bottom - r.top,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}
	if (hwnd_) InvalidateRect(hwnd_, nullptr, TRUE);
}

void PreviewWindow::showStatus(const std::string& text) {
	if (hwnd_) {
		SetWindowTextA(hwnd_, text.c_str());
		UpdateWindow(hwnd_);
	}
}

void PreviewWindow::showSceneMenu() {
	if (!hwnd_ || scenes_.empty()) return;
	HMENU menu = CreatePopupMenu();
	if (!menu) return;
	for (size_t i = 0; i < scenes_.size(); ++i) {
		UINT flags = MF_STRING;
		if (static_cast<int>(i) == currentScene_) flags |= MF_CHECKED;
		AppendMenuA(menu, flags, kSceneMenuBase + static_cast<UINT>(i), scenes_[i].c_str());
	}
	POINT pt;
	GetCursorPos(&pt);
	SetForegroundWindow(hwnd_);
	TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd_, nullptr);
	DestroyMenu(menu);
}

void PreviewWindow::updatePixels() {
	const double* s = df_->data;
	const size_t n = static_cast<size_t>(w_) * h_;
	for (size_t p = 0; p < n; ++p) {
		const uint32_t R = toByte(s[p * 3 + 0]);
		const uint32_t G = toByte(s[p * 3 + 1]);
		const uint32_t B = toByte(s[p * 3 + 2]);
		pixels_[p] = (R << 16) | (G << 8) | B;   // BI_RGB 32bpp: 0x00RRGGBB
	}
}

void PreviewWindow::paint(HDC dc) {
	if (pixels_.empty()) return;
	RECT rc;
	GetClientRect(hwnd_, &rc);
	const int cw = rc.right - rc.left;
	const int ch = rc.bottom - rc.top;
	if (cw <= 0 || ch <= 0) return;
	SetStretchBltMode(dc, COLORONCOLOR);
	StretchDIBits(dc, 0, 0, cw, ch, 0, 0, w_, h_,
		pixels_.data(), &bmi_, DIB_RGB_COLORS, SRCCOPY);
}

void PreviewWindow::refresh() {
	if (!hwnd_ || closed_) return;
	updatePixels();
	HDC dc = GetDC(hwnd_);
	if (dc) {
		paint(dc);
		ReleaseDC(hwnd_, dc);
	}
	if (samples_) {
		const int n = samples_->load(std::memory_order_relaxed);
		std::string title = baseTitle_ + "  spp " + std::to_string(n);
		if (totalSamples_ > 0) {
			title += "/" + std::to_string(totalSamples_);
			if (n >= totalSamples_) title += "  [done]";
		}
		SetWindowTextA(hwnd_, title.c_str());
	}
}

bool PreviewWindow::pump() {
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			closed_ = true;
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return !closed_;
}

LRESULT CALLBACK PreviewWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NCCREATE) {
		auto* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	auto* self = reinterpret_cast<PreviewWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(hwnd, &ps);
		if (self) self->paint(dc);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_ERASEBKGND:
		return 1;   // avoid flicker; we paint the whole client area
	case WM_KEYDOWN:
		if (self) {
			if (wParam < 256) self->keys_[wParam] = true;
			if (wParam == VK_F11) {
				self->setFullscreen(!self->fullscreen_);
				return 0;
			}
			if (wParam == VK_ESCAPE) {
				if (self->fullscreen_) self->setFullscreen(false);
				else PostMessage(hwnd, WM_CLOSE, 0, 0);
				return 0;
			}
			if (wParam == 'P' && !(lParam & (1 << 30))) {   // 'P' saves a snapshot (once per key press)
				self->saveRequest_ = true;
				return 0;
			}
			if (!self->scenes_.empty()) {
				const int count = static_cast<int>(self->scenes_.size());
				if (wParam == VK_OEM_4) {   // '[' previous scene
					self->pendingScene_ = (self->currentScene_ - 1 + count) % count;
					return 0;
				}
				if (wParam == VK_OEM_6) {   // ']' next scene
					self->pendingScene_ = (self->currentScene_ + 1) % count;
					return 0;
				}
			}
		}
		return 0;
	case WM_KEYUP:
		if (self && wParam < 256) self->keys_[wParam] = false;
		return 0;
	case WM_LBUTTONDOWN:
		if (self) {
			self->dragging_ = true;
			self->lastMouseX_ = GET_X_LPARAM(lParam);
			self->lastMouseY_ = GET_Y_LPARAM(lParam);
			SetCapture(hwnd);
		}
		return 0;
	case WM_MOUSEMOVE:
		if (self && self->dragging_) {
			const int x = GET_X_LPARAM(lParam);
			const int y = GET_Y_LPARAM(lParam);
			self->mouseDx_ += x - self->lastMouseX_;
			self->mouseDy_ += y - self->lastMouseY_;
			self->lastMouseX_ = x;
			self->lastMouseY_ = y;
		}
		return 0;
	case WM_LBUTTONUP:
		if (self) {
			self->dragging_ = false;
			ReleaseCapture();
		}
		return 0;
	case WM_MOUSEWHEEL:
		if (self) self->wheel_ += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		return 0;
	case WM_RBUTTONUP:
		if (self) self->showSceneMenu();
		return 0;
	case WM_COMMAND:
		if (self) {
			const int idx = LOWORD(wParam) - kSceneMenuBase;
			if (idx >= 0 && idx < static_cast<int>(self->scenes_.size())) self->pendingScene_ = idx;
		}
		return 0;
	case WM_KILLFOCUS:
		// drop held keys so we don't "stick" moving when focus is lost
		if (self) { for (int i = 0; i < 256; ++i) self->keys_[i] = false; self->dragging_ = false; }
		return 0;
	case WM_CLOSE:
		if (self) self->closed_ = true;
		DestroyWindow(hwnd);
		return 0;
	case WM_DESTROY:
		if (self) self->closed_ = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
