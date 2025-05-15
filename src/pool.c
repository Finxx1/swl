#include <swl.h>

/*
 * MODULE SUMMARY
 *
 * pool.c - provides pool allocators based on memory arenas.
 *
 * Pool allocators are used for all swl_window_t allocations and for the event
 * queue (since stacks don't work well with FIFO). This file should be kept
 * platform independent, and only rely on arenas.
 *
 * AUTHOR: Finxx
 * DATE: 2025.05.10
 */

typedef struct {
	swl_arena_t arena;
	void* free_list;
	ptrdiff_t size;
} swl_pool_t;

swl_pool_t swl_new_pool(ptrdiff_t size) {
	swl_pool_t zero = {0};
	swl_pool_t pool = {0};
	pool.arena = swl_new_arena();
	pool.free_list = swl_push_arena(&pool.arena, 256 * size);
}

void* swl_pool_alloc(swl_pool_t* pool) {
	
}

void swl_pool_free(swl_pool_t* pool, void* ptr) {
	
}

void swl_free_pool(swl_pool_t* pool) {
	
}
