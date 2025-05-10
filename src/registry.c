#include "swl_w32.h"

/*
 * MODULE SUMMARY
 *
 * registry.c - provides all forward-facing APIs. Handles some basic state,
 * like errors and backend selection, and then forwards calls to the backends.
 *
 * When a backend is selected, a vtable is built, and calling platform-specific
 * functions forwards to the function in that vtable.
 *
 * AUTHOR: Finxx
 * DATE: 2025.05.09
 */

static const char* swl_current_error = NULL;

bool swl_error(const char* err) {
	swl_current_error = err;
	return false;
}

const char* swl_get_error(void) {
	return swl_current_error;
}

static swl_backend_t swl_current_backend = SWL_BACKEND_ANY;

static struct {
	swl_window_t* (*swl_create_window)(void);
	bool (*swl_set_window_title)(swl_window_t* window, const char* title);
	bool (*swl_set_window_size)(swl_window_t* window, uint32_t x, uint32_t y);
	bool (*swl_set_window_pos)(swl_window_t* window, uint32_t x, uint32_t y);
	bool (*swl_set_window_resizable)(swl_window_t* window, bool resizable);
	bool (*swl_set_window_transparent)(swl_window_t* window, bool transparent);
	bool (*swl_set_window_borderless)(swl_window_t* window, bool borderless);
	bool (*swl_set_window_visible)(swl_window_t* window, bool visible);
	char* (*swl_get_window_title)(swl_window_t* window);
	bool (*swl_get_window_size)(swl_window_t* window, uint32_t* x, uint32_t* y);
	bool (*swl_get_window_pos)(swl_window_t* window, uint32_t* x, uint32_t* y);
	bool (*swl_get_window_resizable)(swl_window_t* window, bool* resizable);
	bool (*swl_get_window_transparent)(swl_window_t* window, bool* transparent);
	bool (*swl_get_window_borderless)(swl_window_t* window, bool* borderless);
	bool (*swl_get_window_visible)(swl_window_t* window, bool* visible);
	void* (*swl_get_window_handle_raw)(swl_window_t* window);
	bool (*swl_get_event)(swl_event_t* event);
	bool (*swl_wait_event)(void);
	const char* (*swl_get_error)(void);
} swl_vtable;

#define ADD_TO_VTABLE(x, backend) \
swl_vtable. ## x = backend ## _ ## x

bool swl_init(swl_backend_t backend) {
#ifdef SWL_WINDOWS
	if (backend == SWL_BACKEND_ANY) backend = SWL_BACKEND_WIN32;
	if (backend != SWL_BACKEND_WIN32)
#elif defined(SWL_MACOS)
	if (backend == SWL_BACKEND_ANY) backend = SWL_BACKEND_COCOA;
	if (backend != SWL_BACKEND_COCOA)
#elif defined(SWL_UNIX)
	if (backend == SWL_BACKEND_ANY) backend = SWL_BACKEND_X11;
	if (backend != SWL_BACKEND_X11 && backend != SWL_BACKEND_WAYLAND)
#else
#error unimplemented
#endif
	{
		return swl_error("Invalid backend.");
	}
	
	swl_current_backend = backend;

#ifdef SWL_WINDOWS
	ADD_TO_VTABLE(swl_create_window, w32);
	ADD_TO_VTABLE(swl_set_window_title, w32);
	ADD_TO_VTABLE(swl_set_window_size, w32);
	ADD_TO_VTABLE(swl_set_window_pos, w32);
	ADD_TO_VTABLE(swl_set_window_resizable, w32);
	ADD_TO_VTABLE(swl_set_window_transparent, w32);
	ADD_TO_VTABLE(swl_set_window_borderless, w32);
	ADD_TO_VTABLE(swl_set_window_visible, w32);
	ADD_TO_VTABLE(swl_get_window_title, w32);
	ADD_TO_VTABLE(swl_get_window_size, w32);
	ADD_TO_VTABLE(swl_get_window_pos, w32);
	ADD_TO_VTABLE(swl_get_window_resizable, w32);
	ADD_TO_VTABLE(swl_get_window_transparent, w32);
	ADD_TO_VTABLE(swl_get_window_borderless, w32);
	ADD_TO_VTABLE(swl_get_window_visible, w32);
	ADD_TO_VTABLE(swl_get_window_handle_raw, w32);
	ADD_TO_VTABLE(swl_get_event, w32);
	ADD_TO_VTABLE(swl_wait_event, w32);
	return w32_swl_init();
#elif defined(SWL_MACOS)
	ADD_TO_VTABLE(swl_create_window, cocoa);
	ADD_TO_VTABLE(swl_set_window_title, cocoa);
	ADD_TO_VTABLE(swl_set_window_size, cocoa);
	ADD_TO_VTABLE(swl_set_window_pos, cocoa);
	ADD_TO_VTABLE(swl_set_window_resizable, cocoa);
	ADD_TO_VTABLE(swl_set_window_transparent, cocoa);
	ADD_TO_VTABLE(swl_set_window_borderless, cocoa);
	ADD_TO_VTABLE(swl_set_window_visible, cocoa);
	ADD_TO_VTABLE(swl_get_window_title, cocoa);
	ADD_TO_VTABLE(swl_get_window_size, cocoa);
	ADD_TO_VTABLE(swl_get_window_pos, cocoa);
	ADD_TO_VTABLE(swl_get_window_resizable, cocoa);
	ADD_TO_VTABLE(swl_get_window_transparent, cocoa);
	ADD_TO_VTABLE(swl_get_window_borderless, cocoa);
	ADD_TO_VTABLE(swl_get_window_visible, cocoa);
	ADD_TO_VTABLE(swl_get_window_handle_raw, cocoa);
	ADD_TO_VTABLE(swl_get_event, cocoa);
	ADD_TO_VTABLE(swl_wait_event, cocoa);
	return cocoa_swl_init();
#elif defined(SWL_UNIX)
	if (backend == SWL_BACKEND_X11) {
		ADD_TO_VTABLE(swl_create_window, x11);
		ADD_TO_VTABLE(swl_set_window_title, x11);
		ADD_TO_VTABLE(swl_set_window_size, x11);
		ADD_TO_VTABLE(swl_set_window_pos, x11);
		ADD_TO_VTABLE(swl_set_window_resizable, x11);
		ADD_TO_VTABLE(swl_set_window_transparent, x11);
		ADD_TO_VTABLE(swl_set_window_borderless, x11);
		ADD_TO_VTABLE(swl_set_window_visible, x11);
		ADD_TO_VTABLE(swl_get_window_title, x11);
		ADD_TO_VTABLE(swl_get_window_size, x11);
		ADD_TO_VTABLE(swl_get_window_pos, x11);
		ADD_TO_VTABLE(swl_get_window_resizable, x11);
		ADD_TO_VTABLE(swl_get_window_transparent, x11);
		ADD_TO_VTABLE(swl_get_window_borderless, x11);
		ADD_TO_VTABLE(swl_get_window_visible, x11);
		ADD_TO_VTABLE(swl_get_window_handle_raw, x11);
		ADD_TO_VTABLE(swl_get_event, x11);
		ADD_TO_VTABLE(swl_wait_event, x11);
		return x11_swl_init();
	} else {
		ADD_TO_VTABLE(swl_create_window, wl);
		ADD_TO_VTABLE(swl_set_window_title, wl);
		ADD_TO_VTABLE(swl_set_window_size, wl);
		ADD_TO_VTABLE(swl_set_window_pos, wl);
		ADD_TO_VTABLE(swl_set_window_resizable, wl);
		ADD_TO_VTABLE(swl_set_window_transparent, wl);
		ADD_TO_VTABLE(swl_set_window_borderless, wl);
		ADD_TO_VTABLE(swl_set_window_visible, wl);
		ADD_TO_VTABLE(swl_get_window_title, wl);
		ADD_TO_VTABLE(swl_get_window_size, wl);
		ADD_TO_VTABLE(swl_get_window_pos, wl);
		ADD_TO_VTABLE(swl_get_window_resizable, wl);
		ADD_TO_VTABLE(swl_get_window_transparent, wl);
		ADD_TO_VTABLE(swl_get_window_borderless, wl);
		ADD_TO_VTABLE(swl_get_window_visible, wl);
		ADD_TO_VTABLE(swl_get_window_handle_raw, wl);
		ADD_TO_VTABLE(swl_get_event, wl);
		ADD_TO_VTABLE(swl_wait_event, wl);
		return wl_swl_init();
	}
#endif
}

swl_window_t* swl_create_window(void) {
	return swl_vtable.swl_create_window();
}

bool swl_set_window_title(swl_window_t* window, const char* title) {
	return swl_vtable.swl_set_window_title(window, title);
}

bool swl_set_window_size(swl_window_t* window, uint32_t x, uint32_t y) {
	return swl_vtable.swl_set_window_size(window, x, y);
}

bool swl_set_window_pos(swl_window_t* window, uint32_t x, uint32_t y) {
	return swl_vtable.swl_set_window_pos(window, x, y);
}

bool swl_set_window_resizable(swl_window_t* window, bool resizable) {
	return swl_vtable.swl_set_window_resizable(window, resizable);
}

bool swl_set_window_transparent(swl_window_t* window, bool transparent) {
	return swl_vtable.swl_set_window_transparent(window, transparent);
}

bool swl_set_window_borderless(swl_window_t* window, bool borderless) {
	return swl_vtable.swl_set_window_borderless(window, borderless);
}

bool swl_set_window_visible(swl_window_t* window, bool visible) {
	return swl_vtable.swl_set_window_visible(window, visible);
}

char* swl_get_window_title(swl_window_t* window) {
	return swl_vtable.swl_get_window_title(window);
}

bool swl_get_window_size(swl_window_t* window, uint32_t* x, uint32_t* y) {
	return swl_vtable.swl_get_window_size(window, x, y);
}

bool swl_get_window_pos(swl_window_t* window, uint32_t* x, uint32_t* y) {
	return swl_vtable.swl_get_window_pos(window, x, y);
}

bool swl_get_window_resizable(swl_window_t* window, bool* resizable) {
	return swl_vtable.swl_get_window_resizable(window, resizable);
}

bool swl_get_window_transparent(swl_window_t* window, bool* transparent) {
	return swl_vtable.swl_get_window_transparent(window, transparent);
}

bool swl_get_window_borderless(swl_window_t* window, bool* borderless) {
	return swl_vtable.swl_get_window_borderless(window, borderless);
}

bool swl_get_window_visible(swl_window_t* window, bool* visible) {
	return swl_vtable.swl_get_window_visible(window, visible);
}

void* swl_get_window_handle_raw(swl_window_t* window) {
	return swl_vtable.swl_get_window_handle_raw(window);
}

bool swl_get_event(swl_event_t* event) {
	return swl_vtable.swl_get_event(event);
}

bool swl_wait_event(void) {
	return swl_vtable.swl_wait_event();
}
