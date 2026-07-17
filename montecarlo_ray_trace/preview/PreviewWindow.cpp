#include "PreviewWindow.h"

#include <algorithm>

namespace {
const char* kClassName = "MCRT_PreviewWindow";

inline uint32_t toByte(double v) {
	int x = static_cast<int>(v * 255.0 + 0.5);
	if (x < 0) x = 0;
	if (x > 255) x = 255;
	return static_cast<uint32_t>(x);
}
}

PreviewWindow::PreviewWindow(DataFrame* df, std::atomic<int>* progress, const std::string& title)
	: df_(df), progress_(progress),
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
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
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
}

PreviewWindow::~PreviewWindow() {
	if (hwnd_) {
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
	}
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
	if (progress_ && h_ > 0) {
		const int done = progress_->load(std::memory_order_relaxed);
		const int pct = std::min(100, done * 100 / h_);
		std::string title = baseTitle_ + (pct >= 100 ? "  [done]" : "  [" + std::to_string(pct) + "%]");
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
		if (wParam == VK_ESCAPE) {
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
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
