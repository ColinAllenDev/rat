# Makefile - Build configuration for Rat

# Include additional helper definitions
include util.mk

# Compiler & Linker
CC := clang
STD := c11

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

# Flags
CFLAGS := -std=$(STD)
CPPFLAGS := $(INCS) -I$(INC_DIR) -MMD -MP
LDFLAGS := $(LIBS) -lglfw3

# Platform Specific Configuration
UNAME := $(shell uname 2>/dev/null || echo Unknown)
ifeq ($(UNAME),Darwin) ## MacOS
	LDFLAGS += -framework Cocoa -framework OpenGL -framework IOKit 
	CFLAGS += -DGL_SILENCE_DEPRECATION
	export MallocNanoZone=0
endif

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

#-- Clean: Remove build artifacts
.PHONY: clean
clean: 
	@echo "Removing build artifacts..."
	@rm -rf $(BUILD_DIR)

#-- Run: Execute built binary
.PHONY: run
run:
	@if [ -f $(TARGET_DIR)/$(BIN) ]; then \
		./$(TARGET_DIR)/$(BIN); \
	else \
		$(MAKE); \
		./$(TARGET_DIR)/$(BIN); \
	fi

#-- Bear: Generate compilation database
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
