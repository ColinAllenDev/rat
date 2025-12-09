#define GLAD_GL_IMPLEMENTATION
#include "gfx.h"

#include <core/io.h>
#include <util/log.h>

#include <stdio.h>
#include <assert.h>

static unsigned int CompileShaderProgram(unsigned int vsId, unsigned int fsId);
static unsigned int CompileShaderSource(const char* shaderSrc, GLenum shaderType);

// Load shader from files and bind default locations
unsigned int LoadShader(const char* vsFilePath, const char* fsFilePath) 
{
	//--Read shader sources from files
	char* vsSource = NULL;
	char* fsSource = NULL;

	vsSource = ReadFileStr(vsFilePath);
	fsSource = ReadFileStr(fsFilePath);

	// In case of failure, fallback to default shader
	if ((vsSource == NULL) && (fsSource == NULL)) {
		Log(warn, "One or more Shader files invalid, falling back to default.");
		return GFX.State.fallbackShaderId;
	}

	return InitShader(vsSource, fsSource);
}

// Initialize shader from sources
unsigned int InitShader(const char *vsSource, const char *fsSource) 
{
	//--Compile Vertex Shader
	unsigned int vsId = 0;
	vsId = CompileShaderSource(vsSource, GL_VERTEX_SHADER);	
	if (vsId == 0) {
		vsId = GFX.State.fallbackVsId;
		Log(warn, "Vertex shader failed to compile, using fallback (id: %o)", vsId);	
	}

	//--Compile Fragment Shader
	unsigned int fsId = 0;
	fsId = CompileShaderSource(fsSource, GL_FRAGMENT_SHADER);	
	if (fsId == 0) {
		fsId = GFX.State.fallbackFsId;
		Log(warn, "Fragment shader failed to compile, using fallback (id: %u)", fsId);	
	}
	
	//--Compile Shader Program
	unsigned int psId = 0;
	psId = CompileShaderProgram(vsId, fsId);
	if (psId == 0) {
		psId = GFX.State.fallbackShaderId;
		Log(warn, "Shader program failed to compile, using fallback (id: %u)", psId);	
	}
		
	//--Clean source shaders
	// Detach & delete vertex shader
	if (vsId != GFX.State.fallbackVsId) {
		glDetachShader(psId, vsId);	
		glDeleteShader(vsId);
	}

	// Detach & delete fragment shader
	if (fsId != GFX.State.fallbackFsId) {
		glDetachShader(psId, fsId);
		glDeleteShader(fsId);
	}

	return psId;
}

// Hot-reload shader program from source files
unsigned int ReloadShader(unsigned int shader, const char *vsFilePath, const char *fsFilePath) {
	unsigned int rlShader = LoadShader(vsFilePath, fsFilePath);	

	if (rlShader != 0) {
		glDeleteProgram(shader);
		return rlShader;
	} else {
		Log(error, "Failed to reload shader!");
		return 0;
	}
}

// Compile shader from source
static unsigned int CompileShaderSource(const char* shaderSrc, GLenum shaderType)
{
	//--Create shader
	unsigned int shaderId = 0;
	shaderId = glCreateShader(shaderType);

	//--Compile shader
	glShaderSource(shaderId, 1, &shaderSrc, NULL);
	glCompileShader(shaderId);

	//--Result Handling
	int success = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (success == GL_TRUE) {
		Log(info, "Compiled Shader (id=%u)", shaderId);	
	} else {
		// Get shader log):
		char infoLog[2048];
		glGetShaderInfoLog(shaderId, 2048, NULL, infoLog);

		// Print to console
		Log(error, "Failed to compile shader: %s", infoLog);			
		
		// Cleanup
		glDeleteShader(shaderId);
		return 0;
	}

	return shaderId;	
}

// Compile shader program from sources
static unsigned int CompileShaderProgram(unsigned int vsId, unsigned int fsId) 
{
	unsigned int psId = 0;
	// Use fallback program if already using fallback shaders
	if ((vsId == GFX.State.fallbackVsId) && (fsId == GFX.State.fallbackFsId)) {
		return 0;
	// Otherwise, build new shader program from one or more source shaders
	} else if ((vsId > 0) && (fsId > 0)) {
		psId = glCreateProgram();
		
		// Attach vertex and fragment shaders
		glAttachShader(psId, vsId);
		glAttachShader(psId, fsId);

		// Bind default shader attributes
		glBindAttribLocation(psId, GFX_SHADER_DEFAULT_ATTR_POSITION_LOC, GFX_SHADER_DEFAULT_ATTR_POSITION_NAME);
		glBindAttribLocation(psId, GFX_SHADER_DEFAULT_ATTR_COLOR_LOC, GFX_SHADER_DEFAULT_ATTR_COLOR_NAME);
		
		// Link shader program
		glLinkProgram(psId);

		// Get result and handle errors
		int success = 0;
		glGetProgramiv(psId, GL_LINK_STATUS, &success);
		if (success == GL_TRUE)
			Log(debug, "Compiled Shader Program (id=%u)", psId);	
		else 
		{
			// Log result
			char infoLog[2048];
			glGetProgramInfoLog(psId, 2048, NULL, infoLog);
			Log(error, "Failed to link shader program: %s", infoLog);

			// Cleanup
			glDeleteProgram(psId);
			return 0;
		}
	}

	return psId;
}
