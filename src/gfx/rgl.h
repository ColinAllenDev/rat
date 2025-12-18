#ifndef RGL_H
#define RGL_H

#include <stdint.h>
#include <stdbool.h>

#define RGL_DEFAULT_SHADER_POSITION_LOC 0
#define RGL_DEFAULT_SHADER_POSITION_NAME "vertexPosition"
#define RGL_DEFAULT_SHADER_COLOR_LOC 1
#define RGL_DEFAULT_SHADER_COLOR_NAME "vertexColor"

static const char* default_vs_path = "res/shader/default/vs.vert";
static const char* default_fs_path = "res/shader/default/fs.vert";

typedef struct rt_shader rt_shader;

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

#endif /* RGL_H */
