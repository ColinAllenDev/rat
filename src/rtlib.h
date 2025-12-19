#ifndef RAT_LIB_H
#define RAT_LIB_H

#include <core/math.h>

/*
** Custom free() wrapper that prevents dangling pointers
*/
#define rt_free(ptr) do { free(ptr); ptr = NULL; } while(0)

typedef struct vec2 { float x; float y; } vec2;

#endif
