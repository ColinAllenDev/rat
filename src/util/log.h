#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// ANSI Colors
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_WHITE   "\x1b[97m"
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"

// Log Level Prefixes
#define LOG_LEVEL_TRACE_STR	"[TRACE]"
#define LOG_LEVEL_DEBUG_STR	"[DEBUG]"
#define LOG_LEVEL_INFO_STR	"[INFO]"
#define LOG_LEVEL_WARN_STR	"[WARN]"
#define LOG_LEVEL_ERROR_STR	"[ERROR]"
#define LOG_LEVEL_FATAL_STR	"[FATAL]"

// Log Level Colors
#define LOG_LEVEL_TRACE_COLOR	ANSI_BLUE	
#define LOG_LEVEL_DEBUG_COLOR	ANSI_YELLOW	
#define LOG_LEVEL_INFO_COLOR	ANSI_GREEN 	
#define LOG_LEVEL_WARN_COLOR	ANSI_MAGENTA 	
#define LOG_LEVEL_ERROR_COLOR	ANSI_RED	
#define LOG_LEVEL_FATAL_COLOR	ANSI_RED	

//--Log Configuration Macros
// Allow compile-time minimum log level.
// Can be defined before including the header, or via compiler flags
#ifndef LOG_MIN_LEVEL
#define LOG_MIN_LEVEL trace
#endif

// Display conditionals
#define LOG_HIDE_TIME

//--Log Levels
typedef enum { trace, debug, info, warn, error, fatal } log_level;

//--Public Function Macros
#define rt_log(level, ...) _log(level, __VA_ARGS__) 

//--Internal Logging Function
void _log(log_level level, const char* fmt, ...);

#endif /* LOG_H */
#ifdef LOG_IMPL

static const char* level_colors[] = {
	LOG_LEVEL_TRACE_COLOR, 
	LOG_LEVEL_DEBUG_COLOR, 
	LOG_LEVEL_INFO_COLOR, 
	LOG_LEVEL_WARN_COLOR,
	LOG_LEVEL_ERROR_COLOR,
	LOG_LEVEL_FATAL_COLOR,
};

static const char* level_strings[] = {
	LOG_LEVEL_TRACE_STR, 
	LOG_LEVEL_DEBUG_STR, 
	LOG_LEVEL_INFO_STR, 
	LOG_LEVEL_WARN_STR,
	LOG_LEVEL_ERROR_STR,
	LOG_LEVEL_FATAL_STR		
};

void _log(log_level level, const char* fmt, ...) {
	if (level >= LOG_MIN_LEVEL) {
		// --Get Current Time
		time_t tmNow = time(NULL);
		struct tm *tmLocal = localtime(&tmNow);
		char tmBuf[16];
		size_t tmSize = strftime(tmBuf, sizeof(tmBuf), "%H:%M:%S", tmLocal);
		tmBuf[tmSize] = '\0';

		//--Print Log
		// Message Prefix	
		FILE* stream = (level == error) ? stderr : stdout;
		fprintf(stream, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
		  		tmBuf, level_colors[level], level_strings[level], 
		  		__FILE__, __LINE__);
		// Message
		va_list args;
		va_start(args, fmt);
		vfprintf(stream, fmt, args);
		va_end(args);
		fprintf(stream, "\n");
		fflush(stream);
	}
}

#endif /* LOG_IMPL */
