#include "window.h"

#include <util/log.h>
#include <stdlib.h>

/* Platform Implementations */
extern int ext_init_platform(void);
extern void ext_terminate_platform(void);
extern void* ext_create_window(int width, int height, const char* title);
extern void ext_terminate_window(void* window);
extern void ext_draw_window(void* window);
extern int ext_window_should_close(void* window);
extern rt_ivec2 ext_get_framebuffer_size(void* window);

/* Initialize platform libraries */
int rt_init_platform(void)
{
	int status = ext_init_platform();
	return status;
}

void rt_terminate_platform(void) 
{
	ext_terminate_platform();	
}

rt_window* rt_create_window(int width, int height, const char* title) 
{
	rt_window* win = ext_create_window(width, height, title);

	return win;
}

void rt_terminate_window(void* window) 
{
	if (window == NULL) {
		return;
	}

	ext_terminate_window(window);
}

void rt_draw_window(void* window) {
	ext_draw_window(window);
}

int rt_window_should_close(void *window)
{
	return ext_window_should_close(window);	
}

rt_ivec2 rt_get_framebuffer_size(void* window) 
{
	return ext_get_framebuffer_size(window);
}
