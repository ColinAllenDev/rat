#ifndef RAT_GFX_H
#define RAT_GFX_H

#include <glad/gl.h>

static const char* DefaultVertexShader =
"#version 330 core\n"
"in vec3 vp;"
"void main() {"
"gl_Position = vec4(vp, 1.0);"
"}";
static const char* DefaultFragmentShader =
"#version 330 core\n"
"out vec4 frag_colour;"
"void main() {"
"  frag_colour = vec4( 0.5, 0.0, 0.5, 1.0 );"
"}";

#define GFX_SHADER_DEFAULT_ATTR_POSITION_LOC 0
#define GFX_SHADER_DEFAULT_ATTR_COLOR_LOC 1

#define GFX_SHADER_DEFAULT_ATTR_POSITION_NAME "vertexPosition"
#define GFX_SHADER_DEFAULT_ATTR_COLOR_NAME "vertexColor"

typedef struct GfxData {
	struct {
		unsigned int fallbackVsId;
		unsigned int fallbackFsId;
		unsigned int fallbackShaderId;
	} State;
} GfxData;
static GfxData GFX = { 0 };

unsigned int LoadShader(const char* vsFilePath, const char* fsFilePath);
unsigned int InitShader(const char* vsSource, const char* fsSource);
unsigned int ReloadShader(unsigned int shader, const char* vsFilePath, const char* fsFilePath);

#endif /* RAT_GFX_H */
