#ifndef RAT_WINDOW_H
#define RAT_WINDOW_H

#include <stdint.h>

typedef struct rt_window rt_window;
typedef struct rt_ivec2 {int x; int y;} rt_ivec2;

/* Initialize platform libraries */
int rt_init_platform(void);

/* Terminate platform libraries */
void rt_terminate_platform(void);

/* Create a platform window */
rt_window* rt_create_window(int width, int height, const char* title);

/* Destroy a platform window */
void rt_destroy_window(void* window);

/* Draw platform window */
void rt_draw_window(void* window);

/* Window closure conditional */
int rt_window_should_close(void* window);

/* Get window resolution */
rt_ivec2 rt_get_framebuffer_size(void* window);

#endif /* RAT_WINDOW_H */
