#--- Makefile - Build configuration for Rat -#

#--- Helper Variables -----------------------#
LOG 	:= $(shell date -u +%H:%M:%S) \x1b[35m[MAKE]\x1b[0m 

WARN_ALL := -Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wnull-dereference -Wuninitialized
WARN_ERR := -Werror

SAN_ADDR := -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
#--------------------------------------------#

# Compiler & Linker
CC := clang
STD := c23

# Project Structure
BIN := rat
SRC_DIR := src
LIB_DIR := lib
INC_DIR := include
BUILD_DIR := build
TARGET = debug
TARGET_DIR = $(BUILD_DIR)/$(TARGET)

# Project Files
SRCS := $(shell find $(SRC_DIR) -name '*.c')
INCS := $(addprefix -I,$(shell find $(SRC_DIR) -type d))
LIBS := $(addprefix -L,$(shell find $(LIB_DIR) -type d))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(TARGET_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Third Party Libraries
LIB_EXT = .so
L_GL = -lGL -lEGL
L_GLFW = -lglfw3
L_VALGRIND = -L/usr/lib/valgrind

# Third Party Headers
I_VALGRIND = -I/usr/include/valgrind

# Flags
CFLAGS = -std=$(STD)
CPPFLAGS = $(INCS) -I$(INC_DIR) -MMD -MP
LDFLAGS = $(LIBS) $(L_GL) $(L_GLFW)

# Platform Specific Configuration
UNAME := $(shell uname 2>/dev/null || echo Unknown)
## MacOS
ifeq ($(UNAME),Darwin) 
	L_GLFW := -lglfw3osx
	L_GL := -framework Cocoa -framework OpenGL -framework IOKit
	LIB_EXT := .dylib
	CFLAGS += -DGL_SILENCE_DEPRECATION
	export MallocNanoZone=0
endif
## Linux
ifeq ($(UNAME),Linux)
SESSION := $(XDG_SESSION_TYPE)
	ifeq ($(XDG_SESSION_TYPE),wayland)  
	else ifeq ($(XDG_SESSION_TYPE),x11)
	endif

export PKG_CONFIG_PATH=/usr/lib/pkgconfig/
LSAN_OPTIONS := suppressions=.sanitignore
export LSAN_OPTIONS
endif

#-- all: Default Build Target
.PHONY: all
all: debug

#-- debug: Debug Build Target
.PHONY: debug
debug: 
debug: CFLAGS += -g -O0 $(WARN_ALL) $(SAN_ADDR)
debug: LDFLAGS += $(SAN_ADDR)
debug: TARGET := debug
debug: $(TARGET_DIR)/$(BIN)

#-- clean: Remove build artifacts
.PHONY: clean
clean: 
	@echo "${LOG}Removing build artifacts..."
	@rm -rf $(BUILD_DIR)

#-- run: Execute built binary
.PHONY: run
run:
	@if [ -f $(TARGET_DIR)/$(BIN) ]; then \
		./$(TARGET_DIR)/$(BIN); \
	else \
		$(MAKE); \
		./$(TARGET_DIR)/$(BIN); \
	fi

#-- bear: Generate compilation database
.PHONY: bear
bear: clean
	@echo "${LOG}Generating compile_commands.json with bear..."
	@bear -- $(MAKE) -s all
	@echo "${LOG}Compilation database compiled successfully!"

#-- Linker target
$(TARGET_DIR)/$(BIN): $(OBJS)
	@echo "${LOG}Linking $@..."
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "${LOG}Build completed succesfully!"

#-- Compilation target
$(TARGET_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo "${LOG}Compiled $<" 

-include $(DEPS)
