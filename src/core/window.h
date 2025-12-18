#ifndef RAT_WINDOW_H
#define RAT_WINDOW_H

typedef struct vec2 vec2;
typedef struct rt_window rt_window;

/* Function pointer table (vtable) */
typedef struct rt_window_api {
	int 		(*init)(void);
	void 		(*terminate)(void);
	rt_window* 	(*create)(int, int, const char*);
	void		(*destroy)(rt_window*);
	void 		(*draw)(rt_window*);
	bool		(*should_close)(rt_window*);
	vec2 		(*get_size)(rt_window*);
	double		(*get_time)(void);
} rt_window_api;

/* Window backend getters */
rt_window_api* glfw_window_api(void);

#endif
