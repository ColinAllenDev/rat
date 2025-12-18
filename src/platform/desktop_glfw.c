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
	/* #define GLFW_EXPOSE_NATIVE_COCOA */
	/* #include "GLFW/glfw3native.h" */
#endif

/* Initialize GLFW */
int ext_init_platform(void);

/* Terminate GLFW */
void ext_terminate_platform(void);

/* Constructs a GLFW window */
void* ext_create_window(int width, int height, const char* title); 

/* Deconstructs a GLFW window */
void ext_terminate_window(void* window); 

/* Draw the window */
void ext_draw_window(void* window);

/* Returns the close flag of the specified window */
int ext_window_should_close(void* window);

/* Get framebuffer size */
typedef struct ext_vec2 {int x; int y;} ext_vec2;
ext_vec2 ext_get_framebuffer_size(void* window);

/* GLFW error callback */
static void glfw_error_callback(int error, const char* description);

/* GLFW keyboard input callback */
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/*
 * Implementation 
 */

int ext_init_platform(void)
{
	/* Initialize GLFW */
	glfwSetErrorCallback(glfw_error_callback);	

	#if defined(__APPLE__)
		glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
	#endif

	int status = glfwInit();
	if (status == GLFW_FALSE) {
		return 0;
	}

	return status;
}


void ext_terminate_platform(void)
{
	glfwTerminate();
}

void* ext_create_window(int width, int height, const char* title)
{
	/* GLFW Window Hints */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Antialiasing (Performance: 4, Quality: 16) */
    glfwWindowHint(GLFW_SAMPLES, 4);

	/* -- GLFW - Window Creation */
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		return NULL;
	}

	glfwMakeContextCurrent(window);

	int glfwResult = glfwGetError(NULL);
	if ((glfwResult == GLFW_NO_WINDOW_CONTEXT) || (glfwResult == GLFW_PLATFORM_ERROR)) {
		return NULL;
	}

	/* -- GLFW - Window Configuration */
	/* Disable V-Sync by default */
	glfwSwapInterval(0);

	/* -- GLFW - Window Callbacks */
	glfwSetKeyCallback(window, glfw_key_callback);

	return window;
}

void* ext_get_window_context(void) 
{
	if (glfwGetCurrentContext() == NULL) {
		return NULL;
	}
	return NULL;
}

void ext_terminate_window(void* window) 
{
	glfwDestroyWindow(window);
}

void ext_draw_window(void* window)
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int ext_window_should_close(void* window)
{
	return glfwWindowShouldClose(window);
}

ext_vec2 ext_get_framebuffer_size(void* window) 
{
	ext_vec2 fb_size;
	glfwGetFramebufferSize(window, &fb_size.x, &fb_size.y);
	return fb_size;
}

double ext_get_time(void) 
{
	return glfwGetTime();	
}

/* GLFW - Error Callback */
static void glfw_error_callback(int err, const char* description) 
{
	rt_log(error, "(GLFW | E%i): %s", err, description);	
}

/* GLFW - Keyboard Callback */
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/* Debug log */
	rt_log(debug, "GLFW Key Event: (keycode: %i, scancode: %i, mods: %i)", key, scancode, mods);

    /* Close window callack (ESCAPE) */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
