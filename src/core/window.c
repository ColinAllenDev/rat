#include "window.h"

#include <util/log.h>
#include <stdlib.h>

extern void* ext_init_window(int width, int height, const char* title);
extern void* ext_get_window_context(void);
extern void ext_destroy_window(void* window);
extern int ext_window_should_close(void* window);

typedef struct rt_window {
	void* handle;
	const char* title;
	int width;
	int height;
} rt_window;

rt_window* rt_init_window(int width, int height, const char* title) 
{
	/* Initialize a window for a given platform */
	void* handle = ext_init_window(width, height, title);

	/* Construct window object */
	rt_window* win = malloc(sizeof(rt_window));
	win->width = width;
	win->height = height;
	win->title = title;
	win->handle = handle;

	return win;
}

void* rt_get_window_context(void)
{
	return ext_get_window_context();	
}

void rt_destroy_window(rt_window* window) 
{
	if (window->handle == NULL) {
		return;
	}

	ext_destroy_window(window->handle);

	free(window);
}

bool rt_window_should_close(rt_window *window)
{
	return (bool)ext_window_should_close(window->handle);	
}
