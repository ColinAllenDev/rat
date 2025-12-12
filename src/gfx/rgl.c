#include "rgl.h"

#include <core/io.h>
#include <util/log.h>

#include <stdint.h>
#include <assert.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

static uint32_t compile_shader_program(uint32_t vs_id, uint32_t fs_id);
static uint32_t compile_shader_source(const char* shaderSrc, GLenum shaderType);

uint32_t rgl_init(void) 
{
	/* Initialize RGL context */
	rgl_gfx_ctx* ctx = malloc(sizeof(*ctx));	
	if (ctx == NULL) {
		rt_log(fatal, "failed to initialize graphics context!");		
		return 0;
	}

	/* Load default fallback shaders */
	uint32_t fb_shader = rgl_load_shader("res/shaders/default/default.vert", "res/shaders/default/default.frag");
	if (fb_shader == 0) {
		rt_log(error, "Failed to load default shader!");
		return 0;
	}

	/* Assign default shader to graphics context */
	ctx->fallback_shader = fb_shader;
}

uint32_t rgl_load_shader(const char* vs_path, const char* fs_path) 
{
	/* Read shader sources from files */
	char* vs_source = NULL;
	char* fs_src = NULL;

	vs_source = rt_read_file(vs_path);
	fs_src = rt_read_file(fs_path);

	/* In case of failure, fallback to default shader */
	if ((vs_source == NULL) && (fs_src == NULL)) {
		rt_log(warn, "One or more Shader files invalid, falling back to default.");
		return 0;
	}

	return rgl_init_shader(vs_source, fs_src);
}

uint32_t rgl_init_shader(const char *vs_source, const char *fs_src) 
{
	/* Compile Vertex Shader */
	uint32_t vs_id = 0;
	vs_id = compile_shader_source(vs_source, GL_VERTEX_SHADER);	
	if (vs_id == 0) {
		rt_log(warn, "vertex shader failed to compile, using fallback.");	
	}

	/* Compile Fragment Shader */
	uint32_t fs_id = 0;
	fs_id = compile_shader_source(fs_src, GL_FRAGMENT_SHADER);	
	if (fs_id == 0) {
		rt_log(warn, "fragment shader failed to compile (id: %u)", fs_id);	
	}
	
	/* Compile Shader Program */
	uint32_t psId = 0;
	psId = compile_shader_program(vs_id, fs_id);
	if (psId == 0) {
		rt_log(warn, "Shader program failed to compile, using fallback (id: %u)", psId);	
		return 0;
	}
		
	/* Clean source shaders */
	/* Detach & delete vertex shader */
	if (vs_id != GFX.State.fallbackVsId) {
		glDetachShader(psId, vs_id);	
		glDeleteShader(vs_id);
	}

	/* Detach & delete fragment shader */
	if (fs_id != GFX.State.fallbackFsId) {
		glDetachShader(psId, fs_id);
		glDeleteShader(fs_id);
	}

	return psId;
}

uint32_t rgl_reload_shader(uint32_t shader, const char *vs_path, const char *fs_path) {
	uint32_t rlShader = rgl_load_shader(vs_path, fs_path);	

	if (rlShader != 0) {
		glDeleteProgram(shader);
		return rlShader;
	} else {
		rt_log(error, "Failed to reload shader!");
		return 0;
	}
}

/* Compile shader from source */
static uint32_t compile_shader_source(const char* shaderSrc, GLenum shaderType)
{
	/* Create shader */
	uint32_t shaderId = 0;
	shaderId = glCreateShader(shaderType);

	/* Compile shader */
	glShaderSource(shaderId, 1, &shaderSrc, NULL);
	glCompileShader(shaderId);

	/* Result Handling */
	int success = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (success == GL_TRUE) {
		rt_log(info, "Compiled Shader (id=%u)", shaderId);	
	} else {
		/* Get shader log): */
		char infoLog[2048];
		glGetShaderInfoLog(shaderId, 2048, NULL, infoLog);

		/* Print to console */
		rt_log(error, "Failed to compile shader: %s", infoLog);			
		
		/* Cleanup */
		glDeleteShader(shaderId);
		return 0;
	}

	return shaderId;	
}

/* Compile shader program from sources */
static uint32_t compile_shader_program(uint32_t vs_id, uint32_t fs_id) 
{
	uint32_t psId = 0;
	/* Use fallback program if already using fallback shaders */
	if ((vs_id == GFX.State.fallbackVsId) && (fs_id == GFX.State.fallbackFsId)) {
		return 0;
	/* Otherwise, build new shader program from one or more source shaders */
	} else if ((vs_id > 0) && (fs_id > 0)) {
		psId = glCreateProgram();
		
		/* Attach vertex and fragment shaders */
		glAttachShader(psId, vs_id);
		glAttachShader(psId, fs_id);

		/* Bind default shader attributes */
		glBindAttribLocation(psId, GFX_SHADER_DEFAULT_ATTR_POSITION_LOC, GFX_SHADER_DEFAULT_ATTR_POSITION_NAME);
		glBindAttribLocation(psId, GFX_SHADER_DEFAULT_ATTR_COLOR_LOC, GFX_SHADER_DEFAULT_ATTR_COLOR_NAME);
		
		/* Link shader program */
		glLinkProgram(psId);

		/* Get result and handle errors */
		int success = 0;
		glGetProgramiv(psId, GL_LINK_STATUS, &success);
		if (success == GL_TRUE)
			rt_log(debug, "Compiled Shader Program (id=%u)", psId);	
		else 
		{
			/* Log result */
			char infoLog[2048];
			glGetProgramInfoLog(psId, 2048, NULL, infoLog);
			rt_log(error, "Failed to link shader program: %s", infoLog);

			/* Cleanup */
			glDeleteProgram(psId);
			return 0;
		}
	}

	return psId;
}
