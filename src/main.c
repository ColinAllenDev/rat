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

static const float vertex_positions[] = {
    -1,  1, 
     1,  1,
    -1, -1,
     1, -1,
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
        rt_terminate_platform(); 
        exit(EXIT_FAILURE);
    }

    /* Initialize graphics library */
    int gl_version = rgl_init();
    if (gl_version == 0) {
        rt_log(fatal, "failed to load OpenGL");
        rt_terminate_platform();
        rt_terminate_window(window);
        exit(EXIT_FAILURE);
    }
            
    /* Shaders */
    uint32_t shader = rgl_load_shader(
        "res/shaders/lavalamp/vs.vert", 
        "res/shaders/lavalamp/fs.frag"
    );
    
    /* Uniforms */
    int resolution_loc = glGetUniformLocation(shader, "iResolution");
    int time_loc = glGetUniformLocation(shader, "iTime");

    /* Create and bind the vertex array object */
    uint32_t vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    /* Upload vertex data to GPU */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

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
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        /* Draw window */
        rt_draw_window(window);
    }

    /* Cleanup */    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader);
    rgl_terminate();
    rt_terminate_window(window);
    rt_terminate_platform();

    return 0;
}
