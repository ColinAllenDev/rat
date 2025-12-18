/* palloc.h
**
* ---------------------------
* Handle-based Pool Allocator
* ---------------------------
* 
* This demonstrates the "handles vs pointers" pattern:
* ~> Items are stored in a contiguous array (good cache locality)
* ~> External code receives handles, not pointers
* ~> Generation counters detect use-after-free bugs
* 
* Handle layout (32 bits):
* ~> Lower 16 bits: slot index (supports up to 65536 items)
* ~> Upper 16 bits: generation counter (detects stale handles)
**
*/
#ifndef PALLOC_H
#define PALLOC_H

#include <stdlib.h>
#include <stdbool.h>

#define PALLOC_INDEX_BITS 	16
#define PALLOC_GEN_BITS 	16
#define POOL_MAX_SLOTS		(1 << POOL_INDEX_BITS)
#define POOL_INDEX_MASK		(POOL_MAX_SLOTS - 1)
#define POOL_GEN_MASK 		((1 << POOL_GEN_BITS) - 1)

/* res_slot
** Resource pool slot type 
*/
#define SLOT_CNT 64
typedef union res_slot res_slot;
union res_slot {
    res_slot* next;
    char arr[SLOT_CNT];
};

/* res_head
** Contains the address to the next slot array 
** along with a pointer to the head of the next array.
*/
typedef struct res_head res_head;
struct res_head {
    res_slot* list;
    res_head* next;
};

/* res_pool
** A structure representing a resource pool, which contains a pointer 
** to the first slot, and a pointer to the head of the linked list of free slots.
*/
typedef struct res_pool res_pool;
struct res_pool 
{
    res_slot* free_slot;
    res_head* slot_heads;
    size_t slot_sz;
};

/* res_pool_init
** Function for creating a pool with
** an arbitrary number of slots
*/
res_pool* res_pool_init(size_t pool_sz, size_t slot_sz);

/* res_alloc
** Allocates a slot of the resource pool
*/
void* res_alloc(res_pool* pool);

/* res_free
 * Free slot from the resource pool by prepending free slots.
*/
void res_free(res_pool* pool, void* ptr);

/* res_dealloc
** Deallocate the pool from the system 
*/
void res_dealloc(res_pool* pool);

/* res_expand
** Expand the resource pool by n slots 
*/
bool res_expand(res_pool* pool, size_t slots);

#endif /* PALLOC_H */
#ifdef PALLOC_IMPLEMENTATION
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>

res_pool* res_pool_init(size_t pool_sz, size_t slot_sz) 
{
	/* Assert pool_sz and slot_sz are valid */
	if (pool_sz == 0 || slot_sz < sizeof(res_slot)) {
        rt_log(error, "invalid parameters for new resource pool");
        rt_log(error, "pool_sz: %i, slot_sz: %i", pool_sz, slot_sz);
		return NULL;
	} 

    /* Allocate resource pool structure in memory */
    res_pool* pool = malloc(sizeof(res_pool));
    if (pool == NULL) {
        rt_log(error, "failed to allocate pool");
        return NULL;
    }

    /* Allocate the array of pool slots, slots are free by default */
    res_slot* slot_arr = malloc(pool_sz * slot_sz);
    if (slot_arr == NULL) {
        free(pool);
        return NULL;
    }

    /* Build the list of free slots, all but last pointing to adjacent */
    for (size_t i = 0; i < pool_sz - 1; i++) {
        slot_arr[i].next = &slot_arr[i + 1];
    }
    slot_arr[pool_sz - 1].next = NULL;

	/* Allocate the list of slot array heads */
	pool->slot_heads = malloc(sizeof(res_head));
	if (pool->slot_heads == NULL) {
		free(slot_arr);
		free(pool);
		return NULL;
	}

	pool->free_slot = slot_arr;
	pool->slot_heads->next = NULL;
	pool->slot_heads->list = slot_arr;
	pool->slot_sz = slot_sz;

	/* Valigrind */
	VALGRIND_MAKE_MEM_NOACCESS(slot_arr, pool_sz * slot_sz);
	VALGRIND_MAKE_MEM_NOACCESS(pool->slot_heads, sizeof(res_head));
	VALGRIND_MAKE_MEM_NOACCESS(pool, sizeof(res_pool));
	VALGRIND_CREATE_MEMPOOL(pool, 0, 0);
    
    return pool;
}

bool res_expand(res_pool* pool, size_t num_slots)
{
	if (pool == NULL || num_slots == 0) 
		return false;

	VALGRIND_MAKE_MEM_DEFINED(pool, sizeof(res_pool));

	/* Allocate memory for new slot head */
	res_head* slot_head = malloc(sizeof(res_head));
	if (slot_head == NULL)
		return false;

	/* Allocate memory for new slot list */
	res_slot* slot_list = malloc(num_slots * pool->slot_sz);
	if (slot_list == NULL) 
		free(slot_head);
		return false;

	/* Write the addresses of the new slots into the current slot */
	for (size_t i = 0; i < num_slots - 1; i++) {
		*(res_slot**)(num_slots + i * pool->slot_sz) = slot_list + (i + 1) * pool->slot_sz;
	}

	/* Prepend new list of slots to head of free slot list */
	*(res_slot**)(slot_list + (num_slots - 1) * pool->slot_sz) = pool->free_slot;
	pool->free_slot = slot_list;

	/* Prepend slot_head to the list of slot heads stored inside the resource pool */
	slot_head->list = slot_list;
	slot_head->next = pool->slot_heads;
	pool->slot_heads = slot_head;

	/* Valgrind */
	VALGRIND_MAKE_MEM_NOACCESS(slot_list, num_slots * pool->slot_sz);
	VALGRIND_MAKE_MEM_NOACCESS(slot_head, sizeof(res_head));
	VALGRIND_MAKE_MEM_NOACCESS(pool, sizeof(res_pool));

	return true;
}

void* res_alloc(res_pool* pool) 
{
    /* Determine if next slot in pool is free */
    if (pool == NULL || pool->free_slot == NULL) 
		return NULL;
	VALGRIND_MAKE_MEM_DEFINED(pool, sizeof(res_pool));

	if (pool->free_slot == NULL)
		return NULL;
	VALGRIND_MAKE_MEM_DEFINED(pool->free_slot, sizeof(res_slot**));

    /* Set the start of the list to the next free slot before returning */
    res_slot* slot = pool->free_slot;
    pool->free_slot = pool->free_slot->next;

	VALGRIND_MEMPOOL_ALLOC(pool, slot, pool->slot_sz);
	VALGRIND_MAKE_MEM_NOACCESS(pool->free_slot, sizeof(res_slot**));
	VALGRIND_MAKE_MEM_NOACCESS(pool, sizeof(res_pool));

    return slot;
}

void res_free(res_pool* pool, void* ptr) 
{
    /* Assertions */
    if (pool == NULL || ptr == NULL) 
		return;

	VALGRIND_MAKE_MEM_DEFINED(pool, sizeof(res_pool));

    /* Prepend list of free slots with slot at 'ptr' */
	*(void**)ptr = pool->free_slot;
	pool->free_slot = ptr;

	VALGRIND_MAKE_MEM_NOACCESS(pool, sizeof(res_pool));
	VALGRIND_MEMPOOL_FREE(pool, ptr);
}

void res_dealloc(res_pool* pool) 
{
     if (pool == NULL) return;

	VALGRIND_MAKE_MEM_DEFINED(pool, sizeof(res_pool));

	/* Free each slot in the resource pool */
	res_head* slot_head = pool->slot_heads;
	while (slot_head != NULL) {
		VALGRIND_MAKE_MEM_DEFINED(slot_head, sizeof(res_head));

		res_head* next = slot_head->next;
		free(slot_head->list);
		free(slot_head);
		slot_head = next;
	}
	
	VALGRIND_DESTROY_MEMPOOL(pool);
    free(pool);
}
		
#endif /* PALLOC_IMPLEMENTATION */
