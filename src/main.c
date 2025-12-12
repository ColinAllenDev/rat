#define LOG_IMPL
#include <util/log.h>
#include <gfx/rgl.h>
#include <core/window.h>

#include <stdio.h>

#define WIN_INIT_WIDTH 640
#define WIN_INIT_HEIGHT 480
#define WIN_INIT_TITLE "RAT Engine"

int main(void) 
{
    //-- Window
    rt_window_t* window = rt_init_window(WIN_INIT_WIDTH, WIN_INIT_HEIGHT, WIN_INIT_TITLE);

    //--Main Loop
    while (!rt_window_should_close(window)) {
        //--Render Loop
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    //--Cleanup
    rt_destroy_window(window);
    return 0;
}
