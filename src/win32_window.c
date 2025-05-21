#include "swl_w32.h"

#ifdef SWL_WINDOWS

/*
 * MODULE SUMMARY
 *
 * win32_window.c - provides window-based functions on Win32.
 *
 * Until Windows 10, the ANSI Win32 functions did not support UTF-8. Therefore,
 * we always talk to Win32 with the unicode functions. To reduce dependencies,
 * WideCharToMultiByte (and vice versa) is used, rather than using our own text
 * encoding library like iconv.
 *
 * NOTE: The current window title pointer is stored in the beginning of the
 * scratch arena. swl_get_window_title is the ONLY function that can clear it.
 * Right now, we just save the pos value of the arena and restore it at the
 * end of the function. TODO: use temporary arena scopes instead.
 *
 * AUTHOR: Finxx
 * DATE: 2025.05.09
 */

#include <dwmapi.h>

static HMODULE g_hmod;
static swl_pool_t window_pool;
static swl_arena_t scratch;
static DWM_BLURBEHIND blurbehind;

static PWSTR towidechar(swl_arena_t* arena, char* str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if (len == 0) return NULL;
	PWSTR r = swl_push_arena(arena, len);
	if (r == NULL) return NULL;
	int foo = MultiByteToWideChar(CP_UTF8, 0, str, -1, r, len);
	if (foo == 0) {
		swl_pop_arena(arena, len);
		return NULL;
	}
	return r;
}

static char* toutf8(swl_arena_t* arena, PWSTR str) {
	int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	if (len == 0) return NULL;
	char* r = swl_push_arena(arena, len);
	if (r == NULL) return NULL;
	int foo = WideCharToMultiByte(CP_UTF8, 0, str, -1, r, len, NULL, NULL);
	if (foo == 0) {
		swl_pop_arena(arena, len);
		return NULL;
	}
	return r;
}

/* implemented in win32_events.c */
LRESULT CALLBACK wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

	window_pool = swl_new_pool(sizeof(swl_window_t));
	if (window_pool.arena.base == NULL)
		return swl_error("Failed to allocate window pool.");

	scratch = swl_new_arena();
	if (scratch.base == NULL)
		return swl_error("Failed to allocate scratch arena.");

	blurbehind.dwFlags = DWM_BB_BLURREGION | DWM_BB_ENABLE;
	blurbehind.hRgnBlur = CreateRectRgn(-1, -1, 0, 0);

	return w32_swl_events_init();
}

swl_window_t* w32_swl_create_window(void) {
	HWND hWnd = CreateWindowExW(0, L"swl", L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, g_hmod, NULL);

	swl_window_t* r = swl_pool_alloc(&window_pool);
	r->hWnd = hWnd;
	r->transparent = false;
	return r;
}

bool w32_swl_set_window_title(swl_window_t* window, const char* title) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");
	if (title == NULL) return swl_error("Parameter must not be NULL.");

	ptrdiff_t old_pos = scratch.pos;
	PWSTR title_w = towidechar(&scratch, (char*)title);
	if (title_w == NULL) return swl_error("Failed to convert string.");
	BOOL r = SetWindowTextW(window->hWnd, title_w);
	scratch.pos = old_pos;
	if (r != 0) return swl_error("SetWindowTextW failed.");
	return true;
}

bool w32_swl_set_window_size(swl_window_t* window, uint32_t x, uint32_t y) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	DWORD style = GetWindowLongW(window->hWnd, GWL_STYLE);

	RECT rc = {0};
	rc.right = x;
	rc.bottom = y;
	if (AdjustWindowRect(&rc, style, FALSE) == FALSE)
		return swl_error("AdjustWindowRect failed.");

	x = rc.right - rc.left;
	y = rc.bottom - rc.top;

	BOOL r = SetWindowPos(window->hWnd, HWND_TOP, 0, 0, x, y, SWP_NOMOVE | SWP_NOZORDER);
	if (r != 0) return swl_error("SetWindowPos failed.");
	return true;
}

bool w32_swl_set_window_pos(swl_window_t* window, uint32_t x, uint32_t y) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	BOOL r = SetWindowPos(window->hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	if (r != 0) return swl_error("SetWindowPos failed.");
	return true;
}

bool w32_swl_set_window_resizable(swl_window_t* window, bool resizable) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	DWORD style = GetWindowLongW(window->hWnd, GWL_STYLE);
	if (resizable) {
		style |= (WS_MAXIMIZEBOX | WS_THICKFRAME);
	} else {
		style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
	}
	LONG r = SetWindowLongW(window->hWnd, GWL_STYLE, style);
	if (r == 0) return swl_error("SetWindowLongW failed.");
	return true;
}

bool w32_swl_set_window_transparent(swl_window_t* window, bool transparent) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	blurbehind.fEnable = transparent;
	HRESULT hr = DwmEnableBlurBehindWindow(window->hWnd, &blurbehind);
	if (hr != S_OK) return swl_error("DwmEnableBlurBehindWindow failed.");
	window->transparent = transparent;
	return true;
}

bool w32_swl_set_window_borderless(swl_window_t* window, bool borderless) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	DWORD style = GetWindowLongW(window->hWnd, GWL_STYLE);
	if (borderless) {
		style &= ~(WS_OVERLAPPEDWINDOW);
		style |= (WS_POPUP | WS_MINIMIZEBOX);
	} else {
		style &= ~(WS_POPUP | WS_MINIMIZEBOX);
		style |= (WS_OVERLAPPEDWINDOW);
	}
	LONG r = SetWindowLongW(window->hWnd, GWL_STYLE, style);
	if (r == 0) return swl_error("SetWindowLongW failed.");
	return true;
}

bool w32_swl_set_window_visible(swl_window_t* window, bool visible) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	int nCmdShow = visible ? SW_SHOW : SW_HIDE;
	BOOL r = ShowWindow(window->hWnd, nCmdShow);
	if (r == 0) return swl_error("ShowWindow failed.");
	return true;
}

char* w32_swl_get_window_title(swl_window_t* window) {
	if (window == NULL) return (char*)swl_error("Parameter must not be NULL.");

	SetLastError(0);
	int len = GetWindowTextLengthW(window->hWnd);
	if (len == 0 && GetLastError() != 0)
		return (char*)swl_error("GetWindowTextLengthW failed.");

	if (len == 0) return "";

	swl_clear_arena(&scratch);
	PWSTR str = swl_push_arena(&scratch, len + 2);
	int err = GetWindowTextW(window->hWnd, str, len + 2);
	if (err == 0) return (char*)swl_error("GetWindowTextW failed.");
	char* r = toutf8(&scratch, str);
	if (r == NULL) return (char*)swl_error("Window title is not a valid string.");
	return r;
}

bool w32_swl_get_window_size(swl_window_t* window, uint32_t* x, uint32_t* y) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	RECT rc;
	BOOL err = GetClientRect(window->hWnd, &rc);
	if (err == 0) return swl_error("GetClientRect failed.");

	if (x) *x = rc.right - rc.left;
	if (y) *y = rc.bottom - rc.top;

	return true;
}

bool w32_swl_get_window_pos(swl_window_t* window, uint32_t* x, uint32_t* y) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");

	POINT p = { 0, 0 };
	BOOL err = ClientToScreen(window->hWnd, &p);
	if (err == 0) return swl_error("ClientToScreen failed.");
	if (x) *x = p.x;
	if (y) *y = p.y;

	return true;
}

bool w32_swl_get_window_resizable(swl_window_t* window, bool* resizable) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");
	if (resizable == NULL) return swl_error("Parameter must not be NULL.");

	DWORD style = GetWindowLongW(window->hWnd, GWL_STYLE);
	*resizable = (style & (WS_MAXIMIZEBOX | WS_THICKFRAME)) != 0;

	return true;
}

bool w32_swl_get_window_transparent(swl_window_t* window, bool* transparent) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");
	if (transparent == NULL) return swl_error("Parameter must not be NULL.");

	*transparent = window->transparent;

	return true;
}

bool w32_swl_get_window_borderless(swl_window_t* window, bool* borderless) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");
	if (borderless == NULL) return swl_error("Parameter must not be NULL.");

	DWORD style = GetWindowLongW(window->hWnd, GWL_STYLE);
	*borderless = (style & (WS_CAPTION)) != 0;

	return true;
}

bool w32_swl_get_window_visible(swl_window_t* window, bool* visible) {
	if (window == NULL) return swl_error("Parameter must not be NULL.");
	if (visible == NULL) return swl_error("Parameter must not be NULL.");

	*visible = IsWindowVisible(window->hWnd) != 0;

	return true;
}

void* w32_swl_get_window_handle_raw(swl_window_t* window) {
	if (window == NULL) return (void*)swl_error("Parameter must not be NULL.");

	return window->hWnd;
}

#endif /* SWL_WINDOWS */
