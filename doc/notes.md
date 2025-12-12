# C Tips

## Malloc Shorthand
Instead of writing
```c
long_struct_name *foo = malloc(sizeof(long_struct_name))
```
You can write
```c
long_struct_name *foo = malloc(sizeof(*foo))
```

## Nullify Pointers
To avoid dangling pointers, you must nullify when you free them or transfer ownership!
```c
#define rt_free(ptr) do { free(ptr); p = NULL; } while(0)
```

## Memory Management Tips
**Handles instead of Pointers**
Instead of using pointers, resource creation functions should return a 32-bit handle which uniquely identifies a resource object. The 32-bit resource id is split into a 16-bit pool index in the lower bits, and a 16-bit 'generation counter' in the upper bits. 

```c
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

/* Resource pool slots */
typdef struct {
    uint32_t id;
    uint32_t uninit_count;
} rgl_slot_t;
```

The index allows fast pool lookups, and combined with the generation-counter it allows to detect 'dangling accesses' (trying to access an object which no longer exists, and its pool slot has been reused for a new object).

The resource ids are wrapped into a strongly-typed struct so that trying to pass an incompatible resource id is a compile error.

> Note: 
> In the index-handles, only use as many bits as needed for the array index, and use the remaining bits for additional memory safety checks.
> Only convert a handle to a pointer when absolutely needed, and don’t store the pointer anywhere

**Memory Management in Centralized Systems**
Move all memory management into centralized systems (like rendering, physics, animation, etc) with the systems being the sole owner of their memory allocations.

