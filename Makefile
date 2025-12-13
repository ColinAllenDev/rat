# Makefile - Build configuration for Rat

# Include additional helper definitions
include util.mk

# Compiler & Linker
CC := clang
STD := c11
LIB_EXT := .so

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
L_GL := -lGL -lEGL
#L_WAYLAND_GL := lwayland-egl -lwayland-client -lm
L_GLFW := -lglfw -lrt -lm -ldl
L_VALGRIND := -L/usr/lib/valgrind -lcoregrind-amd64-linux -lvex-amd64-linux -lgcc

# Third Party Headers
I_VALGRIND := -I/usr/lib/valgrind

# Flags
CFLAGS := -std=$(STD)
CPPFLAGS := $(INCS) -I$(INC_DIR) -MMD -MP
LDFLAGS := $(LIBS)

# Platform Specific Configuration
UNAME := $(shell uname 2>/dev/null || echo Unknown)
## MacOS
ifeq ($(UNAME),Darwin) 
	L_GLFW = -lglfw
	L_GL := -framework Cocoa -framework OpenGL -framework IOKit
	CFLAGS += -DGL_SILENCE_DEPRECATION
	LIB_EXT = .dylib
	export MallocNanoZone=0
endif
## Linux
ifeq ($(UNAME),Linux)
SESSION := $(XDG_SESSION_TYPE)
	ifeq ($(XDG_SESSION_TYPE),wayland)  
		## Wayland
		## LDFLAGS += $(L_WAYLAND_GL)	
	else ifeq ($(XDG_SESSION_TYPE),x11)
		## X11
	else
		## Unknown
	endif
endif

LDFLAGS += $(L_GL) $(L_GLFW) 
#LDFLAGS += $(L_GL) $(L_GLFW) $(L_VALGRIND) 
#CPPFLAGS += $(I_VALGRIND)

#-- all: Default Build Target
.PHONY: all
all: debug

#-- debug: Debug Build Target
.PHONY: debug
debug: CFLAGS += -g -O0 -DTARGET_DEBUG
debug: CFLAGS += $(CF_STRICT) $(CF_WARN_CORE) $(CF_WARN_VERBOSE)
debug: CFLAGS += $(SF_ADDR) $(SF_FRAME)
debug: LDFLAGS += $(SF_ADDR) $(SF_FRAME)
debug: TARGET := debug
debug: $(TARGET_DIR)/$(BIN)

#-- clean: Remove build artifacts
.PHONY: clean
clean: 
	@echo "Removing build artifacts..."
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
	@echo "Generating compile_commands.json with bear..."
	@bear -- $(MAKE) all
	@echo "Compilation database compiled successfully"

#-- Linker target
$(TARGET_DIR)/$(BIN): $(OBJS)
	@printf "Linking $@..."
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build completed succesfully"

#-- Compilation target
$(TARGET_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	@echo "Compiled $< successfully"

-include $(DEPS)
