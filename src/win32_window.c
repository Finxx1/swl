#include "swl_w32.h"

/*
 * MODULE SUMMARY
 *
 * win32_window.c - provides window-based functions on Win32, including events.
 *
 * Until Windows 10, the ANSI Win32 functions did not support UTF-8. Therefore,
 * we always talk to Win32 with the unicode functions. To reduce dependencies,
 * WideCharToMultiByte (and vice versa) is used, rather than using our own text
 * encoding library like iconv.
 *
 * AUTHOR: Finxx
 * DATE: 2025.05.09
 */

static HMODULE g_hmod;

static LRESULT CALLBACK wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool w32_swl_init(void) {
	g_hmod = GetModuleHandle(NULL);
	if (g_hmod == NULL) return swl_error("Failed to get module handle.");
	
	WNDCLASSEXW wcex = {
		.cbSize = sizeof(wcex),
		.hInstance = g_hmod,
		.hCursor = LoadCursorA(NULL, IDC_ARROW),
		.lpszClassName = L"swl",
		.lpfnWndProc = wndproc,
		.hbrBackground = GetStockObject(BLACK_BRUSH),
	};
	ATOM atom = RegisterClassExW(&wcex);
	if (atom == 0) return swl_error("Failed to register window class.");
}

swl_window_t* w32_swl_create_window(void) {
	HWND hWnd = CreateWindowExW(0, L"swl", L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, g_hmod, NULL);
	
}

bool w32_swl_set_window_title(swl_window_t* window, const char* title) {
	return swl_error("Not implemented.");
}

bool w32_swl_set_window_size(swl_window_t* window, uint32_t x, uint32_t y) {
	return swl_error("Not implemented.");
}

bool w32_swl_set_window_pos(swl_window_t* window, uint32_t x, uint32_t y) {
	return swl_error("Not implemented.");
}

bool w32_swl_set_window_resizable(swl_window_t* window, bool resizable) {
	return swl_error("Not implemented.");
}

bool w32_swl_set_window_transparent(swl_window_t* window, bool transparent) {
	return swl_error("Not implemented.");
}

bool w32_swl_set_window_borderless(swl_window_t* window, bool borderless) {
	return swl_error("Not implemented.");
}

bool w32_swl_set_window_visible(swl_window_t* window, bool visible) {
	return swl_error("Not implemented.");
}

char* w32_swl_get_window_title(swl_window_t* window) {
	(void)swl_error("Not implemented.");
	return false;
}

bool w32_swl_get_window_size(swl_window_t* window, uint32_t* x, uint32_t* y) {
	return swl_error("Not implemented.");
}

bool w32_swl_get_window_pos(swl_window_t* window, uint32_t* x, uint32_t* y) {
	return swl_error("Not implemented.");
}

bool w32_swl_get_window_resizable(swl_window_t* window, bool* resizable) {
	return swl_error("Not implemented.");
}

bool w32_swl_get_window_transparent(swl_window_t* window, bool* transparent) {
	return swl_error("Not implemented.");
}

bool w32_swl_get_window_borderless(swl_window_t* window, bool* borderless) {
	return swl_error("Not implemented.");
}

bool w32_swl_get_window_visible(swl_window_t* window, bool* visible) {
	return swl_error("Not implemented.");
}

void* w32_swl_get_window_handle_raw(swl_window_t* window) {
	swl_error("Not implemented.");
	return NULL;
}

bool w32_swl_get_event(swl_event_t* event) {
	return swl_error("Not implemented.");
}

bool w32_swl_wait_event(void) {
	return swl_error("Not implemented.");
}
