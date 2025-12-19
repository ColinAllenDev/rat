#include "rgl.h"

#include <rtlib.h>
#include <core/io.h>
#include <util/log.h>
#include <stdint.h>
#include <assert.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

static uint32_t compile_shader_program(uint32_t vs_id, uint32_t fs_id);
static uint32_t compile_shader_source(const char* shader_src, GLenum shader_type);

struct rt_shader {
	uint32_t id;
	uint32_t vs_id;
	uint32_t fs_id;
};

static rt_shader* default_shader;

int rgl_init(void) 
{
	/* Initialize OpenGL using GLAD */
	int gl_version = gladLoaderLoadGL();	
	if (gl_version == 0) {
		return 0;
	}

	/* Initialize default vertex shader */
	char* default_vs_src = rt_read_file(default_vs_path); 
	if (!default_vs_src) {
		rt_free(default_vs_src);
		return 0;
	}

	uint32_t default_vs_id = compile_shader_source(default_vs_src, GL_VERTEX_SHADER);
	if (default_vs_id == 0) {
		rt_free(default_vs_src);
		return 0;
	}

	/* Initialize default fragment shader */
	char* default_fs_src = rt_read_file(default_fs_path); 
	if (!default_fs_src) {
		rt_free(default_vs_src);
		rt_free(default_fs_src);
		return 0;
	}

	uint32_t default_fs_id = compile_shader_source(default_fs_src, GL_FRAGMENT_SHADER);
	if (default_fs_id == 0) {
		rt_free(default_vs_src);
		rt_free(default_fs_src);
		return 0;
	}

	/* Initialize default shader program */
	uint32_t default_shader_id  = compile_shader_program(default_vs_id, default_fs_id);
	if (default_shader_id == 0) {
		rt_free(default_vs_src);
		rt_free(default_fs_src);
		return 0;
	}

	/* Uniforms */
    int u_mvp = glGetUniformLocation(default_shader_id, "uMvp");

	/* Allocate memory for default shader */
	default_shader = (rt_shader*)malloc(sizeof(rt_shader));
	default_shader->id = default_shader_id;
	default_shader->vs_id = default_vs_id;
	default_shader->fs_id = default_fs_id;
	
	return gl_version;
}

void rgl_terminate(void) 
{
	/* Free default shader */
	if (default_shader != nullptr) {
		glDetachShader(default_shader->vs_id, default_shader->id);
		glDeleteShader(default_shader->vs_id);
		glDetachShader(default_shader->fs_id, default_shader->id);
		glDeleteShader(default_shader->fs_id);
		glDeleteProgram(default_shader->id);
	}

	/* Unload OpenGL */
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
		rt_log(warn, "vertex shader failed to compile, using fallback");
		vs_id = default_shader->vs_id;
	}

	/* Compile Fragment Shader */
	uint32_t fs_id = compile_shader_source(fs_src, GL_FRAGMENT_SHADER);	
	if (fs_id == 0) {
		rt_log(warn, "fragment shader failed to compile, using fallback");	
		fs_id = default_shader->fs_id;
	}
	
	/* Compile Shader Program */
	uint32_t sp_id = 0;
	if (vs_id == default_shader->vs_id && fs_id == default_shader->fs_id) 
	{
		sp_id = default_shader->id;
	} 
	else if (vs_id > 0 && fs_id > 0) 
	{
		sp_id = compile_shader_program(vs_id, fs_id);

		if (vs_id != default_shader->vs_id) 
		{
			if (sp_id > 0) glDetachShader(sp_id, vs_id);
			glDeleteShader(vs_id);
		}
		
		if (fs_id != default_shader->fs_id) 
		{
			if (sp_id > 0) glDetachShader(sp_id, fs_id);
			glDeleteShader(fs_id);
		}

		if (sp_id == 0)
		{
			rt_log(warn, "shader program failed to compile, using fallback");	
			sp_id = default_shader->id;
		}
	} 

	return sp_id;
}

uint32_t rgl_reload_shader(uint32_t shader, const char *vs_path, const char *fs_path) 
{
	uint32_t r_shader = rgl_load_shader(vs_path, fs_path);	

	if (r_shader != 0) 
	{
		glDeleteProgram(shader);
		return r_shader;
	} 
	else 
	{
		rt_log(error, "failed to reload shader!");
		return 0;
	}
}

/* Compile shader from source */
static uint32_t compile_shader_source(const char* shader_src, GLenum shader_type)
{
	/* Create shader */
	uint32_t shader_id = glCreateShader(shader_type);
	if (shader_id == 0) 
	{
		rt_log(error, "failed to create shader from source: %s", shader_src);
		return 0;
	}

	/* Compile shader */
	glShaderSource(shader_id, 1, &shader_src, NULL);
	glCompileShader(shader_id);

	/* Result Handling */
	int success = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if (success == GL_TRUE) 
	{
		rt_log(info, "Compiled Shader [ID: %u]", shader_id);	
	} 
	else 
	{
		/* Get shader log */
		char info_log[2048];
		glGetShaderInfoLog(shader_id, 2048, NULL, info_log);

		/* Print to console */
		rt_log(error, "failed to compile shader: %s", info_log);			
		
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
	if (vs_id == 0 && fs_id == 1) 
	{
		return 0;
	} 
	/* Otherwise, build new shader program from one or more source shaders */
	else if (vs_id > 0 && fs_id > 0) 
	{
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
		int status;
		glGetProgramiv(sp_id, GL_LINK_STATUS, &status);
		if (status == 0) 
		{
			/* Log result */
			char infoLog[2048];
			glGetProgramInfoLog(sp_id, 2048, NULL, infoLog);
			rt_log(error, "Failed to link shader program: %s", infoLog);

			/* Cleanup */
			glDeleteProgram(sp_id);
			return 0; 
		}
	}

	return sp_id;
}
