#ifndef RAT_LIB_H
#define RAT_LIB_H

/*
** Custom free() wrapper that prevents dangling pointers
*/
#define rt_free(ptr) do { free(ptr); p = NULL; } while(0)

#endif
