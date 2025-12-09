#include "window.h"

#include <util/log.h>
#include <stdlib.h>

extern void* InitPlatformWindow(int width, int height, const char* title);
extern void DestroyPlatformWindow(void* window);
extern int PlatformWindowShouldClose(void* window);

typedef struct Window {
	void* handle;
	const char* title;
	int width;
	int height;
} Window;

Window* InitWindow(int width, int height, const char* title) 
{
	// Initialize a window for a given platform
	void* handle = InitPlatformWindow(width, height, title);

	// Construct window object
	Window* win = malloc(sizeof(Window));
	win->width = width;
	win->height = height;
	win->title = title;
	win->handle = handle;

	return win;
}

void DestroyWindow(Window* window) 
{
	if (window->handle == NULL) {
		return;
	}

	DestroyPlatformWindow(window->handle);

	free(window);
}

bool WindowShouldClose(Window *window)
{
	return (bool)PlatformWindowShouldClose(window->handle);	
}
