#define LOG_IMPL
#include <util/log.h>
#include <gfx/gfx.h>
#include <core/window.h>

#include <stdio.h>

#define WIN_INIT_WIDTH 640
#define WIN_INIT_HEIGHT 480
#define WIN_INIT_TITLE "RAT Engine"

int main(void) 
{
    //-- Window
    Window* window = InitWindow(WIN_INIT_WIDTH, WIN_INIT_HEIGHT, WIN_INIT_TITLE);

    //--Main Loop
    while (!WindowShouldClose(window)) {
        //--Render Loop
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    //--Cleanup
    DestroyWindow(window);
    return 0;
}
