#define RAT_LOG_IMPLEMENTATION
#include <util/log.h>
#include <gfx/rgl.h>
#include <core/window.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>

#define WIN_INIT_WIDTH 640
#define WIN_INIT_HEIGHT 480
#define WIN_INIT_TITLE "Rat"

static const float clip_space_verticies[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f,  1.0f,
    -1.0f,  1.0f
};

int main(void) 
{
    /* Initialize Platform */
    int platform_status = rt_init_platform();
    if (platform_status == 0) {
        rt_log(fatal, "failed to initialize platform");
        exit(EXIT_FAILURE);
    }

    /* Initialize window */
    void* window = rt_create_window(WIN_INIT_WIDTH, WIN_INIT_HEIGHT, WIN_INIT_TITLE);
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
    rt_log(info, "OpenGL Version: %i", gl_version);
    rgl_log_params();

    /* Configure graphics library */
    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Enable back-face culling */
    glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK);    /* Cull back-facing triangles */
    glFrontFace(GL_CCW);    /* Counter-clockwise winding = front face */

    /* Shaders */
    uint32_t shader = rgl_load_shader(
        "res/shaders/gradient/vs.vert", 
        "res/shaders/gradient/fs.frag"
    );
    
    /* Uniforms */
    int resolution_loc = glGetUniformLocation(shader, "iResolution");
    int time_loc = glGetUniformLocation(shader, "iTime");
    int mouse_loc = glGetUniformLocation(shader, "iMouse");

    /* Create and bind the vertex array object */
    uint32_t vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    /* Upload vertex data to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(clip_space_verticies), clip_space_verticies, GL_STATIC_DRAW);

    /* Tell OpenGL how to interpret the vertex data 
     * (2 floats per vertex, starting at offset 0) */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    /* Main Loop */
    while (!rt_window_should_close(window)) {
        /* Get framebuffer size */
        rt_ivec2 fb_size = rt_get_framebuffer_size(window);
        glViewport(0, 0, fb_size.x, fb_size.y);

        /* Clear frame */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Use shader */
        glUseProgram(shader);

        /* Pass uniform values to the shader */
        glUniform3f(resolution_loc, (float)fb_size.x, (float)fb_size.y, 1.0f);
        glUniform1f(time_loc, (float)glfwGetTime());

        // Draw cube
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        /* Draw window */
        rt_draw_window(window);
    }

    /* Cleanup */    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader);
    rt_destroy_window(window);
    rt_terminate_platform();

    return 0;
}
