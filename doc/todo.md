# Platform Abstraction Code Review

## Overview

This document contains a review of the RAT engine's platform abstraction layer, specifically focusing on the window interface and GLFW desktop implementation.

---

## What You're Doing Well

1. **Link-time polymorphism** - Using `extern` declarations is the right approach for C
2. **Opaque pointers** - `void*` hides platform details effectively
3. **Minimal API surface** - Simple, focused interface
4. **Zero runtime overhead** - No vtables or function pointers
5. **Clear naming conventions** - `rt_*` for public, `ext_*` for platform

---

## Issues & Improvements

### 1. Type Safety: Use Opaque Struct Instead of `void*`

**Current Problem** (src/core/window.h:6-12):
```c
void* rt_init_window(int width, int height, const char* title);
void rt_destroy_window(void* window);
bool rt_window_should_close(void* window);
```

**Issue**: `void*` allows passing **any pointer type** without compiler warnings:
```c
FILE* file = fopen("test.txt", "r");
rt_destroy_window(file);  // Compiles! Undefined behavior at runtime
```

**Idiomatic C Solution** - Use an **incomplete/opaque struct type**:

```c
// window.h
typedef struct rt_window rt_window;  // Opaque type declaration

rt_window* rt_init_window(int width, int height, const char* title);
void rt_destroy_window(rt_window* window);
bool rt_window_should_close(const rt_window* window);
```

**Benefits**:
- Type safety: Can't pass wrong pointer types
- Self-documenting: `rt_window*` is clearer than `void*`
- Standard C idiom: Used by stdlib (`FILE*`), POSIX, OpenSSL, etc.
- Still opaque: Implementation hidden in .c file

**How it works**: The struct is declared but never defined in the header. Platform code casts `rt_window*` to the real type (`GLFWwindow*`).

---

### 2. Resource Management: Window Initialization is Fragile

**Problem** (src/platform/desktop_glfw.c:52-85):
```c
if (glfwInit() == GLFW_FALSE) {
    return NULL;
}
// ... window creation ...
if (!window) {
    glfwDestroyWindow(window);  // Destroying NULL window!
    glfwTerminate();
    return NULL;
}
```

**Issues**:
- Calling `glfwDestroyWindow(NULL)` is unnecessary
- `glfwTerminate()` called on each window creation failure
- No separation between library init and window creation
- Can't create multiple windows (each init calls `glfwInit`)

**Idiomatic Solution** - Separate initialization from window creation:

```c
// Add to window.h
bool rt_init(void);           // Initialize platform once
void rt_shutdown(void);       // Clean up platform once
rt_window* rt_create_window(int w, int h, const char* title);
void rt_destroy_window(rt_window* window);
```

**Usage pattern**:
```c
// main.c
rt_init();  // Once at startup

rt_window* win1 = rt_create_window(640, 480, "Window 1");
rt_window* win2 = rt_create_window(800, 600, "Window 2");

rt_destroy_window(win1);
rt_destroy_window(win2);

rt_shutdown();  // Once at exit
```

This matches the GLFW design and most platform libraries.

---

### 3. Missing Error Reporting

**Problem**: Functions return `NULL` with no way to know *why* they failed:
```c
void* window = rt_init_window(640, 480, "Game");
if (!window) {
    // Was it: GLFW init failure? OpenGL version mismatch?
    // Out of memory? No idea!
}
```

**Idiomatic C Solutions**:

**Option A: Error code via out-parameter**
```c
typedef enum {
    RT_OK = 0,
    RT_ERROR_PLATFORM_INIT,
    RT_ERROR_WINDOW_CREATE,
    RT_ERROR_GL_CONTEXT,
    RT_ERROR_GL_VERSION
} rt_error;

rt_window* rt_create_window(int w, int h, const char* title, rt_error* err);
```

**Option B: Get last error (like Windows/OpenGL)**
```c
rt_window* rt_create_window(int w, int h, const char* title);
rt_error rt_get_last_error(void);  // Get error from failed call
const char* rt_error_string(rt_error err);
```

---

### 4. Const Correctness

**Issue** (src/core/window.c:32-35):
```c
bool rt_window_should_close(void *window)  // Should be const!
{
    return (bool)ext_window_should_close(window);
}
```

This function **reads** window state but doesn't modify it. Mark it `const`:

```c
bool rt_window_should_close(const rt_window* window);
bool ext_window_should_close(const void* window);
```

**Benefits**:
- Documents intent: "This function won't modify your window"
- Enables compiler optimizations
- Allows usage with const window pointers

---

### 5. Return Type Inconsistency

**Problem** (src/core/window.c:6-9):
```c
extern int ext_window_should_close(void* window);  // Returns int
// But...
bool ext_window_should_close(void* window) {       // Implements bool
    return glfwWindowShouldClose(window);
}
```

**Fix**: Make the contract match the implementation:
```c
extern bool ext_window_should_close(const void* window);
```

---

### 6. Missing Null Checks

**Unsafe** (src/core/window.c:28-30):
```c
void rt_draw_window(void* window) {
    ext_draw_window(window);  // What if window is NULL?
}
```

Either:
1. Document that passing NULL is undefined behavior (common in C)
2. Add defensive checks with assertions
3. Add null checks and return error codes

**Standard C Idiom - Use Assertions**:
```c
#include <assert.h>

void rt_draw_window(rt_window* window) {
    assert(window != NULL && "rt_draw_window: window is NULL");
    ext_draw_window(window);
}
```

Assertions:
- Catch bugs in debug builds
- Compile away in release (`-DNDEBUG`)
- Document preconditions

---

### 7. Unused Dead Code

**Problem** (src/platform/desktop_glfw.c:88-94):
```c
void* ext_get_window_context(void)
{
    if (glfwGetCurrentContext() == NULL) {
        return NULL;
    }
    return NULL;  // Always returns NULL!
}
```

This function is:
- Not declared in the `extern` contract
- Always returns NULL
- Never called

**Fix**: Remove it or implement properly if needed.

---

## More Idiomatic Patterns

### Alternative: Function Pointer Table (Optional)

If you want runtime platform switching (e.g., Vulkan vs OpenGL):

```c
// window.h
typedef struct {
    rt_window* (*create)(int, int, const char*);
    void (*destroy)(rt_window*);
    void (*draw)(rt_window*);
    bool (*should_close)(const rt_window*);
} rt_platform_api;

void rt_init(const rt_platform_api* platform);
```

**Trade-offs**:
- Runtime platform selection
- Can switch backends dynamically
- Slight overhead (indirect calls)
- More complex

For your use case, the **extern symbol approach is better** - simpler and faster.

---

### Naming: Consider Dropping the Prefix on Internals

Current:
```c
extern void* ext_init_window(...);  // 'ext_' visible in .c file only
```

**Idiomatic Alternative** - Use static functions + single platform entry point:
```c
// desktop_glfw.c
static GLFWwindow* platform_create_window(int w, int h, const char* title) {
    // Implementation
}

// External symbol (what core links against)
void* ext_init_window(int w, int h, const char* title) {
    return platform_create_window(w, h, title);
}
```

**Benefits**:
- Platform code can use natural names internally
- `static` functions are clearer about scope
- `ext_*` only appears at the boundary

---

## Recommended Refactor (Minimal Changes)

### Priority 1: Type Safety
Replace `void*` with `typedef struct rt_window rt_window;`

### Priority 2: Separate Init/Shutdown
```c
bool rt_init(void);
void rt_shutdown(void);
rt_window* rt_create_window(...);
void rt_destroy_window(rt_window*);
```

### Priority 3: Const Correctness
```c
bool rt_window_should_close(const rt_window* window);
```

### Priority 4: Error Handling
Add error codes/messages for debugging

---

## Example: Improved Header

```c
// window.h
#ifndef RAT_WINDOW_H
#define RAT_WINDOW_H

#include <stdbool.h>

/* Opaque window handle */
typedef struct rt_window rt_window;

/* Error codes */
typedef enum {
    RT_OK = 0,
    RT_ERROR_PLATFORM_INIT,
    RT_ERROR_WINDOW_CREATE,
    RT_ERROR_GL_CONTEXT
} rt_error;

/* Platform lifecycle */
bool rt_init(void);
void rt_shutdown(void);
rt_error rt_get_last_error(void);

/* Window management */
rt_window* rt_create_window(int width, int height, const char* title);
void rt_destroy_window(rt_window* window);

/* Window operations */
void rt_draw_window(rt_window* window);
bool rt_window_should_close(const rt_window* window);

#endif /* RAT_WINDOW_H */
```

---

## Final Verdict

Your abstraction is **well-architected** for a C project. The extern symbol pattern is solid and performant. The main improvements are around:

1. **Type safety** (opaque struct vs void*)
2. **Resource management** (separate init/shutdown)
3. **Error handling** (know why things fail)
4. **Const correctness** (document read-only operations)

These changes would make your code more maintainable, safer, and align better with standard C library design patterns (POSIX, SDL, GLFW itself, etc.).
