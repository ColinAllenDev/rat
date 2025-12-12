#ifndef RAT_WINDOW_H
#define RAT_WINDOW_H

#include <stdbool.h>

typedef struct rt_window_t rt_window_t;

rt_window_t* rt_init_window(int width, int height, const char* title);
void rt_destroy_window(rt_window_t* window);
bool rt_window_should_close(rt_window_t* window);

#endif /* RAT_WINDOW_H */
