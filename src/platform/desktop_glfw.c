#include <gfx/rgl.h>
#include <util/log.h>

#include <stdbool.h>

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

/* Windows specific configuration */
#if defined(_WIN32)
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_NATIVE_INCLUDE_NONE
	#include <GLFW/glfw3native.h>
#endif
/* Unix specific configuration */
#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#endif
/* MacOS specific configuration */
#if defined(__APPLE__)
	/* #define GLFW_EXPOSE_NATIVE_COCOA */
	/* #include "GLFW/glfw3native.h" */
#endif

/* Constructs a GLFW window */
void* _init_platform_window(int width, int height, const char* title); 

/* Deconstructs a GLFW window */
void _destroy_platform_window(void* window); 

/* Returns the close flag of the specified window */
bool _platform_window_should_close(void* window);

/* GLFW error callback */
static void glfw_error_callback(int error, const char* description);

/* GLFW keyboard input callback */
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/* Implementation */
void* _init_platform_window(int width, int height, const char* title)
{
	/* Initialize GLFW */
	glfwSetErrorCallback(glfw_error_callback);	

	#if defined(__APPLE__)
		glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
	#endif

	if (glfwInit() == GLFW_FALSE) {
		return NULL;
	}

	/* -- GLFW Window Hints */
	/* Reset window hints to defaults */
	glfwDefaultWindowHints();
    /* Antialiasing (Performance: 4, Quality: 16) */
    glfwWindowHint(GLFW_SAMPLES, 4);

	/* -- GLFW - Window Creation */
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

	/* -- GLAD - Load OpenGL Extensions */
	if (gladLoadGL(glfwGetProcAddress) == 0) {
		return NULL;
	}

	/* -- GLFW - Window Configuration */
	/* Disable V-Sync by default */
	glfwSwapInterval(0);

	/* -- GLFW - Window Callbacks */
	glfwSetKeyCallback(window, glfw_key_callback);

	return window;
}

void _destroy_platform_window(void* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool _platform_window_should_close(void* window)
{
	return glfwWindowShouldClose(window);
}

static void glfw_error_callback(int err, const char* description) 
{
	rt_log(error, "(GLFW | E%i): %s", err, description);	
}

/* GLFW - Keyboard Callback */
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /* Close window callack (ESCAPE) */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	
	/* Debug log */
	rt_log(debug, "GLFW Key Event: (keycode: %i, scancode: %i, mods: %i)", key, scancode, mods);
}
