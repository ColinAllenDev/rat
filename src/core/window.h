#ifndef RAT_WINDOW_H
#define RAT_WINDOW_H

#include <stdbool.h>

typedef struct Window Window;

Window* InitWindow(int width, int height, const char* title);
void DestroyWindow(Window* window);
bool WindowShouldClose(Window* window);

#endif /* RAT_WINDOW_H */
