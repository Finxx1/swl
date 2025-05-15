#ifndef _SWL_H_
#define _SWL_H_

#ifdef _WIN32
#define SWL_WINDOWS
#elif defined(__MACH__)
#define SWL_MACOS
#else
#define SWL_UNIX
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct swl_window_t swl_window_t;

typedef enum {
	SWL_EVENT_NULL,
	SWL_EVENT_QUIT,
	SWL_EVENT_KEYDOWN,
	SWL_EVENT_KEYUP,
	SWL_EVENT_MOUSEMOVE,
	SWL_EVENT_MOUSEDOWN,
	SWL_EVENT_MOUSEUP,
	SWL_EVENT_RESIZING,
	SWL_EVENT_RESIZED,
	SWL_EVENT_PAINT,
} swl_event_type_t;

typedef enum {
	SWL_BACKEND_ANY,
	SWL_BACKEND_WIN32,
	SWL_BACKEND_COCOA,
	SWL_BACKEND_X11,
	SWL_BACKEND_WAYLAND,
} swl_backend_t;

typedef struct {
	swl_event_type_t type;
	union {
		struct {
			uint32_t keycode;
			uint32_t scancode;
			bool repeat;
		} key;
		struct {
			uint32_t x;
			uint32_t y;
			uint32_t oldx;
			uint32_t oldy;
		} mouse_move;
		struct {
			uint32_t button;
		} mouse_button;
		struct {
			uint32_t w;
			uint32_t h;
			uint32_t oldw;
			uint32_t oldh;
		} resize;
		struct {
			uint32_t x0;
			uint32_t y0;
			uint32_t x1;
			uint32_t y1;
		} paint;
	};
} swl_event_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool swl_init(swl_backend_t backend);
swl_backend_t swl_get_backend(void);
swl_window_t* swl_create_window(void);
bool swl_set_window_title(swl_window_t* window, const char* title);
bool swl_set_window_size(swl_window_t* window, uint32_t x, uint32_t y);
bool swl_set_window_pos(swl_window_t* window, uint32_t x, uint32_t y);
bool swl_set_window_resizable(swl_window_t* window, bool resizable);
bool swl_set_window_transparent(swl_window_t* window, bool transparent);
bool swl_set_window_borderless(swl_window_t* window, bool borderless);
bool swl_set_window_visible(swl_window_t* window, bool visible);
char* swl_get_window_title(swl_window_t* window);
bool swl_get_window_size(swl_window_t* window, uint32_t* x, uint32_t* y);
bool swl_get_window_pos(swl_window_t* window, uint32_t* x, uint32_t* y);
bool swl_get_window_resizable(swl_window_t* window, bool* resizable);
bool swl_get_window_transparent(swl_window_t* window, bool* transparent);
bool swl_get_window_borderless(swl_window_t* window, bool* borderless);
bool swl_get_window_visible(swl_window_t* window, bool* visible);
void* swl_get_window_handle_raw(swl_window_t* window);
bool swl_get_event(swl_event_t* event);
bool swl_wait_event(void);
const char* swl_get_error(void);

#ifdef INTERNAL
bool swl_error(const char* err);

typedef struct {
	void* base;
	ptrdiff_t pos;
	ptrdiff_t reserved;
	ptrdiff_t committed;
} swl_arena_t;

swl_arena_t swl_new_arena(void);
void* swl_push_arena(swl_arena_t* arena, ptrdiff_t amnt);
void swl_pop_arena(swl_arena_t* arena, ptrdiff_t amnt);
void swl_free_arena(swl_arena_t* arena);

typedef struct {
	swl_arena_t pool_arena;
	void* free_list;
} swl_pool_t;

swl_pool_t swl_new_pool(ptrdiff_t size);
void* swl_pool_alloc(swl_pool_t* pool);
void swl_pool_free(swl_pool_t* pool, void* ptr);
void swl_free_pool(swl_pool_t* pool);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_SWL_H_ */
