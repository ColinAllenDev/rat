#include "rgl.h"
#include "rtlib.h"

#include <core/io.h>
#include <util/log.h>

#include <stdint.h>
#include <assert.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

GLenum rgl_params[] = {
	#define X(name)name,
	RGL_PARAM
	#undef X
}; 
static const char* rgl_params_str[] = {
	#define X(name)#name,
	RGL_PARAM
	#undef X
};

static uint32_t compile_shader_program(uint32_t vs_id, uint32_t fs_id);
static uint32_t compile_shader_source(const char* shader_src, GLenum shader_type);

int rgl_init(void) 
{
	/* Initialize OpenGL using GLAD */
	int gl_version = gladLoaderLoadGL();	
	if (gl_version == 0) {
		return 0;
	}
	
	/* Configure OpenGL */
	/* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Enable back-face culling */
    //glEnable(GL_CULL_FACE); 
    //glCullFace(GL_BACK);    /* Cull back-facing triangles */
    //glFrontFace(GL_CCW);    /* Counter-clockwise winding = front face */

	/* Log OpenGL Info */
	rt_log(debug, "OpenGL Version: %i", gl_version);
	
	return gl_version;
}

void rgl_terminate(void) 
{
	gladLoaderUnloadGL();
}

uint32_t rgl_load_shader(const char* vs_path, const char* fs_path) 
{
	/* Read shader sources from files */
	char* vs_src = rt_read_file(vs_path);
	char* fs_src = rt_read_file(fs_path);
	if ((vs_src == NULL) && (fs_src == NULL)) {
		rt_log(error, "failed to load shader files");

		rt_free(vs_src);
		rt_free(fs_src);
		return 0;
	}

	/* Initialize shader program */
	uint32_t sp_id = rgl_init_shader(vs_src, fs_src);
	if (sp_id == 0) {
		rt_log(error, "failed to initialize shader");
	}

	/* Cleanup */
	rt_free(vs_src);
	rt_free(fs_src);
	return sp_id;
}

uint32_t rgl_init_shader(const char *vs_src, const char *fs_src) 
{
	/* Compile Vertex Shader */
	uint32_t vs_id = compile_shader_source(vs_src, GL_VERTEX_SHADER);	
	if (vs_id == 0) {
		rt_log(error, "vertex shader failed to compile");
	}

	/* Compile Fragment Shader */
	uint32_t fs_id = compile_shader_source(fs_src, GL_FRAGMENT_SHADER);	
	if (fs_id == 0) {
		rt_log(error, "fragment shader failed to compile");	
		glDeleteShader(vs_id);
		glDeleteShader(fs_id);
		return 0;
	}
	
	/* Compile Shader Program */
	uint32_t sp_id = compile_shader_program(vs_id, fs_id);
	if (sp_id == 0) {
		rt_log(error, "shader program failed to compile");	
		glDeleteShader(vs_id);
		glDeleteShader(fs_id);
		return 0;
	}
		
	/* Detach & delete vertex shader */
	glDetachShader(sp_id, vs_id);	
	glDeleteShader(vs_id);

	/* Detach & delete fragment shader */
	glDetachShader(sp_id, fs_id);
	glDeleteShader(fs_id);

	return sp_id;
}

uint32_t rgl_reload_shader(uint32_t shader, const char *vs_path, const char *fs_path) {
	uint32_t r_shader = rgl_load_shader(vs_path, fs_path);	

	if (r_shader != 0) {
		glDeleteProgram(shader);
		return r_shader;
	} else {
		rt_log(error, "failed to reload shader!");
		return 0;
	}
}

void rgl_log_params(void) 
{
	size_t num_params = sizeof(rgl_params) / sizeof(rgl_params[0]); for (size_t i = 0; i < num_params; i++) {
		int tmp = 0;
		glGetIntegerv(rgl_params[i], &tmp);
		rt_log(info, "%s %i", rgl_params_str[i], tmp);
	}
}

/* Compile shader from source */
static uint32_t compile_shader_source(const char* shader_src, GLenum shader_type)
{
	/* Create shader */
	uint32_t shader_id = glCreateShader(shader_type);
	if (shader_id == 0) {
		rt_log(error, "failed to create shader from source! %s", shader_src);
		return 0;
	}

	/* Compile shader */
	glShaderSource(shader_id, 1, &shader_src, NULL);
	glCompileShader(shader_id);

	/* Result Handling */
	int success = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if (success == GL_TRUE) {
		rt_log(info, "Compiled Shader (id: %u)", shader_id);	
	} else {
		/* Get shader log */
		char infoLog[2048];
		glGetShaderInfoLog(shader_id, 2048, NULL, infoLog);

		/* Print to console */
		rt_log(error, "failed to compile shader!");			
		rt_log(error, "%s", infoLog);			
		
		/* Cleanup */
		glDeleteShader(shader_id);
		return 0;
	}

	return shader_id;	
}

/* Compile shader program from sources */
static uint32_t compile_shader_program(uint32_t vs_id, uint32_t fs_id) 
{
	uint32_t sp_id = 0;
	/* Use fallback program if already using fallback shaders */
	if ((vs_id == 0) && (fs_id == 1)) {
		return 0;
	/* Otherwise, build new shader program from one or more source shaders */
	} else if ((vs_id > 0) && (fs_id > 0)) {
		sp_id = glCreateProgram();
		
		/* Attach vertex and fragment shaders */
		glAttachShader(sp_id, vs_id);
		glAttachShader(sp_id, fs_id);

		/* Bind default shader attributes */
		glBindAttribLocation(sp_id, RGL_DEFAULT_SHADER_POSITION_LOC, RGL_DEFAULT_SHADER_POSITION_NAME);
		glBindAttribLocation(sp_id, RGL_DEFAULT_SHADER_COLOR_LOC, RGL_DEFAULT_SHADER_COLOR_NAME);
		
		/* Link shader program */
		glLinkProgram(sp_id);

		/* Get result and handle errors */
		int success = 0;
		glGetProgramiv(sp_id, GL_LINK_STATUS, &success);
		if (success == GL_TRUE)
			rt_log(debug, "Compiled Shader Program (id=%u)", sp_id);	
		else 
		{
			/* Log result */
			char infoLog[2048];
			glGetProgramInfoLog(sp_id, 2048, NULL, infoLog);
			rt_log(error, "Failed to link shader program: %s", infoLog);

			/* Cleanup */
			glDeleteProgram(sp_id);
			return 0; }
	}

	return sp_id;
}


