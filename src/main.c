#define RAT_LOG_IMPLEMENTATION
#include <util/log.h>
#include <gfx/rgl.h>
#include <core/window.h>

#include <glad/gl.h>

#include <stdlib.h>
#include <time.h>

#define WIN_INIT_WIDTH 640
#define WIN_INIT_HEIGHT 480
#define WIN_INIT_TITLE "RAT Engine"

int main(void) 
{
    /* Initialize window */
    void* window = rt_init_window(WIN_INIT_WIDTH, WIN_INIT_HEIGHT, WIN_INIT_TITLE);
    if (window == NULL) {
        rt_log(fatal, "failed to create window");
        exit(EXIT_FAILURE);
    }

    /* Initialize graphics library */
    int gl_version = rgl_init();
    if (gl_version == 0) {
        rt_log(fatal, "failed to load OpenGL");
        exit(EXIT_FAILURE);
    }
    rt_log(debug, "OpenGL Version: %i", gl_version);

    /* Shaders */
    uint32_t shader = rgl_load_shader(
        "res/shaders/cube/cube.vert", 
        "res/shaders/cube/cube.frag"
    );
    int u_time = glGetUniformLocation(shader, "u_time");

    uint32_t vao = 0;
    glGenVertexArrays(1, &vao);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Enable back-face culling */
    glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK);    /* Cull back-facing triangles */
    glFrontFace(GL_CCW);    /* Counter-clockwise winding = front face */

    /* Start update timer */
    clock_t start_time = clock();

    /* Main Loop */
    while (!rt_window_should_close(window)) {
        /* Clear frame */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Use shader */
        glUseProgram(shader);
        
        /* Get elapsed time */
        clock_t current_time = clock();
        float elapsed_time = (float)(current_time - start_time)/CLOCKS_PER_SEC;
        glUniform1f(u_time, elapsed_time);

        // Draw cube
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /* Draw window */
        rt_draw_window(window);
    }

    /* Cleanup */    
    rt_destroy_window(window);

    return 0;
}
