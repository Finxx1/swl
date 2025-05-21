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

/* type for chunks in the pool */
typedef struct chunk {
	struct chunk* next_chunk;
} chunk;

#define POOL_SIZE 256

swl_pool_t swl_new_pool(ptrdiff_t size) {
	swl_pool_t zero = {0};
	swl_pool_t pool = {0};

	if (size < sizeof(void*)) return zero;

	pool.arena = swl_new_arena();
	if (pool.arena.base == NULL) return zero;
	pool.free_list = swl_push_arena(&pool.arena, POOL_SIZE * size);
	if (pool.free_list == NULL) {
		swl_free_arena(&pool.arena);
		return zero;
	}

	/* set up free list */
	chunk* chunks = pool.free_list;
	for (size_t i = 0; i < POOL_SIZE - 1; i++) {
		chunks->next_chunk = SWL_ADD_OFFSET(chunks, size);
		chunks = SWL_ADD_OFFSET(chunks, size);
	}
	chunks->next_chunk = NULL;

	return pool;
}

void* swl_pool_alloc(swl_pool_t* pool) {
	if (pool == NULL) return NULL;

	if (pool->free_list == NULL) {
		ptrdiff_t size = pool->size;
		/* create a new free list */
		pool->free_list = swl_push_arena(&pool->arena, POOL_SIZE * size);
		if (pool->free_list == NULL) return NULL;

		chunk* chunks = pool->free_list;
		for (size_t i = 0; i < POOL_SIZE - 1; i++) {
			chunks->next_chunk = SWL_ADD_OFFSET(chunks, size);
			chunks = SWL_ADD_OFFSET(chunks, size);
		}
		chunks->next_chunk = NULL;
	}

	chunk* r = pool->free_list;
	pool->free_list = r->next_chunk;
	r->next_chunk = NULL;
	return r;
}

void swl_pool_free(swl_pool_t* pool, void* ptr) {
	if (pool == NULL) return;
	if (ptr == NULL) return;

	chunk* r = ptr;
	r->next_chunk = pool->free_list;
	pool->free_list = r;
}

void swl_free_pool(swl_pool_t* pool) {
	swl_free_arena(&pool->arena);
	pool->free_list = NULL;
}

