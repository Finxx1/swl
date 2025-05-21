#include "swl_w32.h"

#ifdef SWL_WINDOWS

/*
 * MODULE SUMARY
 *
 * win32_events.c - provides event translation from Win32 to SWL.
 *
 * The event queue is a singly-linked list allocated using a pool allocator.
 * We maintain our own event queue because one operating system event may
 * translate into multiple SWL events.
 *
 * AUTHOR: Finxx
 * DATE: 2025.05.21
 */

typedef struct queue {
	swl_event_t event;
	struct queue* next;
} queue;

static swl_pool_t event_pool;
static queue* oldest_event;

bool w32_swl_events_init(void) {
	event_pool = swl_new_pool(sizeof(queue));
	if (event_pool.arena.base == NULL) return swl_error("Failed to allocate event pool.");

	oldest_event = NULL;

	return true;
}

LRESULT CALLBACK wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool w32_swl_get_event(swl_event_t* event) {
	if (event == NULL) return swl_error("Parameter must not be NULL.");

	return true;
}

bool w32_swl_wait_event(void) {
	if (oldest_event != NULL) return true;
	return WaitMessage() != 0;
}

#endif /* SWL_WINDOWS */
