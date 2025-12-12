#include "window.h"

#include <util/log.h>
#include <stdlib.h>

extern void* _init_platform_window(int width, int height, const char* title);
extern void _destroy_platform_window(void* window);
extern int _platform_window_should_close(void* window);

typedef struct rt_window_t {
	void* handle;
	const char* title;
	int width;
	int height;
} rt_window_t;

rt_window_t* rt_init_window(int width, int height, const char* title) 
{
	// Initialize a window for a given platform
	void* handle = _init_platform_window(width, height, title);

	// Construct window object
	rt_window_t* win = malloc(sizeof(rt_window_t));
	win->width = width;
	win->height = height;
	win->title = title;
	win->handle = handle;

	return win;
}

void rt_destroy_window(rt_window_t* window) 
{
	if (window->handle == NULL) {
		return;
	}

	_destroy_platform_window(window->handle);

	free(window);
}

bool rt_window_should_close(rt_window_t *window)
{
	return (bool)_platform_window_should_close(window->handle);	
}
