#define RAT_LOG_IMPLEMENTATION
#include <util/log.h>
#include <gfx/rgl.h>
#include <core/window.h>

#include <stdlib.h>

#define WIN_INIT_WIDTH 640
#define WIN_INIT_HEIGHT 480
#define WIN_INIT_TITLE "RAT Engine"

int main(void) 
{
    /* Initialize window */
    rt_window* window = rt_init_window(WIN_INIT_WIDTH, WIN_INIT_HEIGHT, WIN_INIT_TITLE);

    /* Initialize graphics library */
    int gl_version = rgl_init();
    if (gl_version == 0) {
        rt_log(fatal, "failed to load OpenGL");
        exit(EXIT_FAILURE);
    }
    rt_log(debug, "OpenGL Version: %i", gl_version);

    /* Shaders */
    rgl_load_shader("res/shaders/default/default.vert", "res/shaders/default/default.frag");

    /* Main Loop */
    while (!rt_window_should_close(window)) {
        /* Render Loop */
        /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */
    }

    /* --Cleanup */
    rt_destroy_window(window);
    return EXIT_SUCCESS;
}
