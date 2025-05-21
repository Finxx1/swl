#include "swl_w32.h"

#ifdef SWL_WINDOWS

/*
 * MODULE SUMMARY
 *
 * win32_mem.c - provides arena allocators on Windows.
 *
 * Because we are going without a standard library, we need to make our own
 * allocators. We use VirtualAlloc to reserve and commit pages.
 *
 * AUTHOR: Finxx
 * DATE: 2025.05.10
 */

#define ARENA_RESERVE 32 * 1024 * 1024
#define ARENA_INITIAL_COMMIT 32 * 1024

swl_arena_t swl_new_arena(void) {
	swl_arena_t zero = {0};
	swl_arena_t arena = {0};
	arena.base = VirtualAlloc(NULL, ARENA_RESERVE, MEM_RESERVE, PAGE_READWRITE);
	if (arena.base == NULL) return zero;
	
	void* foo = VirtualAlloc(arena.base, ARENA_INITIAL_COMMIT, MEM_COMMIT, PAGE_READWRITE);
	if (foo == NULL) {
		VirtualFree(arena.base, 0, MEM_RELEASE);
		return zero;
	}
	
	arena.pos = 0;
	arena.reserved = ARENA_RESERVE;
	arena.committed = ARENA_INITIAL_COMMIT;
	return arena;
}

void* swl_push_arena(swl_arena_t* arena, ptrdiff_t amnt) {
	if (arena == NULL) return NULL;
	if (amnt == 0) return NULL;
	
	while (arena->pos + amnt >= arena->committed) {
		ptrdiff_t newsize = arena->committed * 2;
		void* foo = VirtualAlloc(arena, newsize, MEM_COMMIT, PAGE_READWRITE);
		if (foo == NULL) {
			return NULL;
		}
		arena->committed = newsize;
	}
	uint8_t* r = arena->base;
	r += arena->pos;
	arena->pos += amnt;
	return r;
}

void swl_pop_arena(swl_arena_t* arena, ptrdiff_t amnt) {
	if (arena == NULL) return;
	if (amnt == 0) return;
	
	arena->pos -= amnt;
}

void swl_clear_arena(swl_arena_t* arena) {
	if (arena == NULL) return;

	arena->pos = 0;
}

void swl_free_arena(swl_arena_t* arena) {
	if (arena == NULL) return;
	
	VirtualFree(arena, 0, MEM_RELEASE);
}

#endif /* SWL_WINDOWS */
