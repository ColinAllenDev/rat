#include "window.h"

#include <util/log.h>
#include <stdlib.h>

extern void* ext_init_window(int width, int height, const char* title);
extern void ext_destroy_window(void* window);
extern void ext_draw_window(void* window);
extern int ext_window_should_close(void* window);

void* rt_init_window(int width, int height, const char* title) 
{
	/* Initialize a window for a given platform */
	void* win = ext_init_window(width, height, title);

	return win;
}

void rt_destroy_window(void* window) 
{
	if (window == NULL) {
		return;
	}

	ext_destroy_window(window);
}

void rt_draw_window(void* window) {
	ext_draw_window(window);
}

bool rt_window_should_close(void *window)
{
	return (bool)ext_window_should_close(window);	
}
