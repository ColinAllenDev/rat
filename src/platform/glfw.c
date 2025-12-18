#include <rtlib.h>
#include <core/window.h>
#include <gfx/rgl.h>
#include <util/log.h>
#include <stdint.h>
#define GLFW_INCLUDE_NONE
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
	#define GLFW_EXPOSE_NATIVE_COCOA
	#include <GLFW/glfw3native.h>
#endif

struct rt_window {
	GLFWwindow* handle;
	int width;
	int height;
};

/* GLFW error callback */
static void glfw_error_callback(int error, const char* description);

/* GLFW keyboard input callback */
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

static int glfw_init(void)
{
	/** Initialize GLFW **/
	glfwSetErrorCallback(glfw_error_callback);	

	#if defined(__APPLE__)
		glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
	#endif

	int status = glfwInit();
	if (status == GLFW_FALSE) {
		return 0;
	}

	/** Configure GLFW **/
	/* OpenGL Version */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Antialiasing (Performance: 4, Quality: 16) */
    glfwWindowHint(GLFW_SAMPLES, 4);

	return status;
}

static void glfw_terminate(void)
{
	glfwTerminate();
}

static rt_window* glfw_create_window(int width, int height, const char* title)
{
	/* Create window */
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		return NULL;
	}

	glfwMakeContextCurrent(window);

	int glfwResult = glfwGetError(NULL);
	if ((glfwResult == GLFW_NO_WINDOW_CONTEXT) || (glfwResult == GLFW_PLATFORM_ERROR)) {
		return NULL;
	}

	/* Set Input Callback */
	glfwSetKeyCallback(window, glfw_key_callback);
	
	/* Allocate memory for window structure */
	rt_window* tmp = (rt_window*)malloc(sizeof(rt_window));

	/* Initialize window structure */
	tmp->handle = window;
	tmp->width = width;
	tmp->height = height;

	return tmp;
}

static void glfw_destroy_window(rt_window* window) 
{
	glfwDestroyWindow(window->handle);
}

static void glfw_draw_window(rt_window* window)
{
	glfwSwapBuffers(window->handle);
	glfwPollEvents();
}

static bool glfw_should_close(rt_window* window)
{
	return glfwWindowShouldClose(window->handle);
}

static vec2 glfw_get_size(rt_window* window) 
{
	vec2 fb_size;
	glfwGetFramebufferSize(window->handle, (int*)&fb_size.x, (int*)&fb_size.y);
	return fb_size;
}

static double glfw_get_time(void) 
{
	return glfwGetTime();	
}

static void glfw_error_callback(int err, const char* description) 
{
	rt_log(error, "(GLFW | E%i): %s", err, description);	
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/* Debug log */
	rt_log(debug, "GLFW Key Event: (keycode: %i, scancode: %i, mods: %i)", key, scancode, mods);

    /* Close window callack (ESCAPE) */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static rt_window_api glfw_api = {
	.init	= glfw_init,
	.terminate = glfw_terminate,
	.create = glfw_create_window,
	.destroy = glfw_destroy_window,
	.draw = glfw_draw_window,
	.should_close = glfw_should_close,
	.get_size = glfw_get_size,
	.get_time = glfw_get_time
};

rt_window_api* glfw_window_api(void) {
	return &glfw_api;
}
