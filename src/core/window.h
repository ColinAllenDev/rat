#ifndef RAT_WINDOW_H
#define RAT_WINDOW_H

#include <stdbool.h>

void* rt_init_window(int width, int height, const char* title);

void rt_destroy_window(void* window);

void rt_draw_window(void* window);

bool rt_window_should_close(void* window);

#endif /* RAT_WINDOW_H */
