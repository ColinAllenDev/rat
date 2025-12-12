#ifndef RGL_H
#define RGL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "log.h"

#define RGL_DEFAULT_SHADER_POSITION_LOC 0
#define RGL_DEFAULT_SHADER_POSITION_NAME "vertexPosition"
#define RGL_DEFAULT_SHADER_COLOR_LOC 1
#define RGL_DEFAULT_SHADER_COLOR_NAME "vertexColor"

/*
** Resource id types
** - rgl_buffer_t: vertex and index buffers
*/
typedef struct rgl_buffer_t { uint32_t id; } rgl_buffer_t;

/*
** rgl_bindings_t
** Defines the resource bindings for the next draw call
*/
typedef struct rgl_bindings_t {
    rgl_buffer_t vertex_buffers[8];
    int vertex_buffer_offsets[8];
    rgl_buffer_t index_buffer;
    int index_buffer_offset;
} rgl_bindings_t;

/* rgl_res_state
** The current state of a resource in its pool.
*/
typedef enum rgl_res_state {
    RGL_RES_STATE_INIT,
    RGL_RES_STATE_ALLOC,
    RGL_RES_STATE_VALID,
    RGL_RES_STATE_FAILED,
    RGL_RES_STATE_INVALID,
} rgl_res_state;

/* rgl_slot
** Resource pool slot type 
*/
#define SLOT_CNT 64
typedef union rgl_slot rgl_slot;
union rgl_slot {
    rgl_slot* next;
    char arr[SLOT_CNT];
};


/* rgl_pool
** Resource pool type 
*/
typedef struct rgl_pool rgl_pool;
struct rgl_pool 
{
    rgl_slot* free_slot;
    rgl_slot* slot_arr;
};

/* rgl_init_pool 
** Function for creating a pool with
** an arbitrary number of slots
*/
rgl_pool* rgl_new_pool(size_t pool_sz) 
{
    /* Allocate resource pool structure in memory */
    rgl_pool* pool = malloc(sizeof(rgl_pool));
    if (pool == NULL) {
        rt_log(error, "failed to allocate pool");
        return NULL;
    }
    
    /* Allocate the pool slots, slots are free by default */
    pool->slot_arr = pool->free_slot = malloc(pool_sz * sizeof(rgl_slot));
    if (pool->slot_arr == NULL) {
        free(pool);
        return NULL;
    }

    /* Build the list of free slots, all but last pointing to adjacent */
    for (size_t i = 0; i < pool_sz - 1; i++) {
        pool->slot_arr[i].next = &pool->slot_arr[i + 1];
    }
    pool->slot_arr[pool_sz - 1].next = NULL;
    
    return pool;
}

/* rgl_alloc
** Allocates a slot of the resource pool
*/
void* rgl_alloc(rgl_pool* pool) 
{
    /* Determine if next slot in pool is free */
    if (pool == NULL || pool->free_slot == NULL)
        return NULL;

    /* Set the start of the list to the next free slot before returning */
    rgl_slot* slot = pool->free_slot;
    pool->free_slot = pool->free_slot->next;
    return slot;
}

/* rgl_free
 * Free slot from the resource pool by prepending free slots.
*/
void rgl_free(rgl_pool* pool, void* ptr) 
{
    /* Assertions */
    if (pool == NULL || ptr == NULL)
        return;

    /* Prepend list of free slots with slot at 'ptr' */
    rgl_slot* fslot = ptr;
    fslot->next = pool->free_slot;
    /* Set tail of free slots to newly freed slot */
    pool->free_slot = fslot;
}

/* rgl_dealloc
** Deallocate the pool from the system 
*/
void rgl_dealloc(rgl_pool* pool) 
{
     if (pool == NULL)
        return;

    free(pool->slot_arr);
    free(pool);
}

/* rgl_gfx_ctx
** Graphics context 
*/
typedef struct {
	uint32_t fallback_shader;
} rgl_gfx_ctx;

/* 
** Initialize graphics context
*/
uint32_t rgl_init(void);

/* 
** Initialize shader from sources 
*/
uint32_t rgl_init_shader(const char* vs_src, const char* fs_src);

/* 
** Load shader from files and bind default locations
*/
uint32_t rgl_load_shader(const char* vs_path, const char* fs_path);

/* 
** Hot-reload shader program from source files 
*/
uint32_t rgl_reload_shader(unsigned int shader, const char* vs_path, const char* fs_path);

#endif /* RGL_H */
