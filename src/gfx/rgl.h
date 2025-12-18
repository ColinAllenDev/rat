#ifndef RGL_H
#define RGL_H

#include <stdint.h>
#include <stdbool.h>

#define RGL_DEFAULT_SHADER_POSITION_LOC 0
#define RGL_DEFAULT_SHADER_POSITION_NAME "vertexPosition"
#define RGL_DEFAULT_SHADER_COLOR_LOC 1
#define RGL_DEFAULT_SHADER_COLOR_NAME "vertexColor"

/* OpenGL Parameters */
#define RGL_PARAM \
	X(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) \
    X(GL_MAX_CUBE_MAP_TEXTURE_SIZE) \
    X(GL_MAX_DRAW_BUFFERS) \
    X(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS) \
    X(GL_MAX_TEXTURE_IMAGE_UNITS) \
    X(GL_MAX_TEXTURE_SIZE) \
    X(GL_MAX_VARYING_FLOATS) \
    X(GL_MAX_VERTEX_ATTRIBS) \
    X(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS) \
    X(GL_MAX_VERTEX_UNIFORM_COMPONENTS) \
    X(GL_MAX_VIEWPORT_DIMS) \
    X(GL_STEREO)

/*
** Initialize OpenGL
*/
int rgl_init(void);

/*
** Terminate OpenGL
*/
void rgl_terminate(void);

/* 
** Load shader from files and bind default locations
*/
uint32_t rgl_load_shader(const char* vs_path, const char* fs_path);

/* 
** Initialize shader from sources 
*/
uint32_t rgl_init_shader(const char* vs_src, const char* fs_src);

/* 
** Hot-reload shader program from source files 
*/
uint32_t rgl_reload_shader(unsigned int shader, const char* vs_path, const char* fs_path);

/*
** Load model (currently just GLTF models supported)
*/
uint32_t rgl_load_model(const char* model_path);

/* 
** Log parameters from GL context
*/
void rgl_log_params(void);


#endif /* RGL_H */
