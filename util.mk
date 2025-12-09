# util.mk
# Contains helper definitions for colors, sanitizer flags and diagnostic flags

## Colors
LOG_INFO 	= \x1b[32m[INFO]\x1b[0m 
LOG_WARN 	= \x1b[33m[WARN]\x1b[0m
LOG_DEBUG 	= \x1b[34m[DEBUG]\x1b[0m 
LOG_FATAL 	= \x1b[31m[FATAL]\x1b[0m

## Strict language conformance flags
# -pedantic enables all warnings demanded by strict ISO C compliance
# -pedantic-errors treats these warnings as errors
CF_STRICT = -pedantic -pedantic-errors

## Core diagnostic flags
# -Wall enables a comprehensive set of warnings about questionable constructs
# -Wnull-dereference attempts to warn about dereferencing NULL pointers
# -Wuninitialized warns about using variables before they're initialized
CF_WARN_CORE = -Wall -Wnull-dereference -Wuninitialized

## Verbose diagnostic flags
# -Wextra adds even more warnings that Wall doesn't cover, like unused parameters
# -Wconversion warns about implicit type conversions that may alter values (like int to char)
# -Wsign-conversion specifically warns about conversions between signed and unsigned types
# -Wdouble-promotion warns when a float is implicitly promoted to double
# -Wshadow warns when a local variable shadows another variable, which often indicates confusion
# -Wundef warns if an undefined macro is used in an #if directive (prevents typos)
# -Wstrict-prototypes requires that all function declarations specify argument types
# -Wpointer-arith warns about pointer arithmetic on void* or function pointers
# -Wcast-align warns when you cast a pointer in a way that increases alignment requirements
# -Wwrite-strings treats string literals as const, catching attempts to modify them
# -Wformat=2 enables strict checking of printf/scanf format strings (level 2 is most strict)
CF_WARN_VERBOSE = -Wextra -Wconversion -Wsign-conversion -Wdouble-promotion -Wshadow -Wundef -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wwrite-strings

## Enforce Diagnostic Flags
# -Werror treats ALL warnings as errors
CF_WARN_ENFORCE = -Werror

## Sanitizer Flags
# `address` detects memory errors like buffer overflows, use-after-free, and memory leaks
SF_ADDR = -fsanitize=address
# `undefined` catches undefined behaviors like int overflow, div by 0, invalid shifts
SF_UNDEF = -fsanitize=undefined
# `no-omit-frame-pointer` keeps frame pointers for better strack traces in error reports 
# `no-optimize-sibling-calls` prevents tail call optimization which can confuse stack traces
SF_FRAME = -fno-omit-frame-pointer -fno-optimize-sibling-calls

