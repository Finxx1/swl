#ifndef _SWL_W32_H_
#define _SWL_W32_H_
#ifdef _WIN32

#include <Windows.h>
#include <stdbool.h>

struct swl_window_t {
	HWND hWnd;
	bool transparent;
};

#include <swl.h>

bool w32_swl_init(void);
swl_window_t* w32_swl_create_window(void);
bool w32_swl_set_window_title(swl_window_t* window, const char* title);
bool w32_swl_set_window_size(swl_window_t* window, uint32_t x, uint32_t y);
bool w32_swl_set_window_pos(swl_window_t* window, uint32_t x, uint32_t y);
bool w32_swl_set_window_resizable(swl_window_t* window, bool resizable);
bool w32_swl_set_window_transparent(swl_window_t* window, bool transparent);
bool w32_swl_set_window_borderless(swl_window_t* window, bool borderless);
bool w32_swl_set_window_visible(swl_window_t* window, bool visible);
char* w32_swl_get_window_title(swl_window_t* window);
bool w32_swl_get_window_size(swl_window_t* window, uint32_t* x, uint32_t* y);
bool w32_swl_get_window_pos(swl_window_t* window, uint32_t* x, uint32_t* y);
bool w32_swl_get_window_resizable(swl_window_t* window, bool* resizable);
bool w32_swl_get_window_transparent(swl_window_t* window, bool* transparent);
bool w32_swl_get_window_borderless(swl_window_t* window, bool* borderless);
bool w32_swl_get_window_visible(swl_window_t* window, bool* visible);
void* w32_swl_get_window_handle_raw(swl_window_t* window);

bool w32_swl_events_init(void);
bool w32_swl_get_event(swl_event_t* event);
bool w32_swl_wait_event(void);

#endif /* _WIN32 */
#endif /* !_SWL_W32_H_ */
