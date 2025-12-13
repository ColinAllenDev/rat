#ifndef RAT_WINDOW_H
#define RAT_WINDOW_H

#include <stdbool.h>

typedef struct rt_window rt_window;

rt_window* rt_init_window(int width, int height, const char* title);
void* rt_get_window_context(void);
void rt_destroy_window(rt_window* window);
bool rt_window_should_close(rt_window* window);

#endif /* RAT_WINDOW_H */
