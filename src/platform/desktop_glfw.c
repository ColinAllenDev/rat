#include <gfx/gfx.h>
#include <util/log.h>

#include <stdbool.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//--Native Window Handlers
//------------------------------------------------
// Win32 specific configuration
#if defined(_WIN32)
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_NATIVE_INCLUDE_NONE
	#include <GLFW/glfw3native.h>
#endif
// Unix specific configuration
#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#endif
// MacOS specific configuration
#if defined(__APPLE__)
	//#define GLFW_EXPOSE_NATIVE_COCOA
	//#include "GLFW/glfw3native.h"
#endif

// Constructs a GLFW window
void* InitPlatformWindow(int width, int height, const char* title); 

// Deconstructs a GLFW window
void DestroyPlatformWindow(void* window); 

// Returns the close flag of the specified window
bool PlatformWindowShouldClose(void* window);

// GLFW error callback
static void GlfwErrorCallback(int error, const char* description);

// GLFW keyboard input callback
static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

//-- Implementation
//------------------------------------------------
void* InitPlatformWindow(int width, int height, const char* title)
{
	//-- Initialize GLFW
	glfwSetErrorCallback(GlfwErrorCallback);	

	#if defined(__APPLE__)
		glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
	#endif

	if (glfwInit() == GLFW_FALSE) {
		return NULL;
	}

	//-- GLFW Window Hints
	// Reset window hints to defaults
	glfwDefaultWindowHints();
    // Antialiasing (Performance: 4, Quality: 16)
    glfwWindowHint(GLFW_SAMPLES, 4);

	//-- GLFW - Window Creation
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	int glfwResult = glfwGetError(NULL);
	if ((glfwResult == GLFW_NO_WINDOW_CONTEXT) || (glfwResult == GLFW_PLATFORM_ERROR)) {
		return NULL;
	}

	//-- GLAD - Load OpenGL Extensions
	if (gladLoadGL(glfwGetProcAddress) == 0) {
		return NULL;
	}

	//-- GLFW - Window Configuration
	// Disable V-Sync by default
	glfwSwapInterval(0);

	//-- GLFW - Window Callbacks
	glfwSetKeyCallback(window, GlfwKeyCallback);

	return window;
}

void DestroyPlatformWindow(void* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool PlatformWindowShouldClose(void* window)
{
	return glfwWindowShouldClose(window);
}

static void GlfwErrorCallback(int err, const char* description) 
{
	Log(error, "(GLFW | E%i): %s", err, description);	
}

// GLFW - Keyboard Callback
static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Close window callack (ESCAPE)
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
