# ================================================
# Makefile - Windows + MinGW compatible
# ================================================

TARGET      := stellar.exe
BUILD_DIR   := build
BIN_DIR     := bin
SRC_DIR     := src
INC_DIR     := inc
LIB_DIR 	:= lib

CXX         := g++
CXXFLAGS    := -std=c++20 -Wall -Wextra -O2
INCLUDES    := -I$(INC_DIR)
LIBRARY		:= -L$(LIB_DIR)
LDFLAGS     := 

# Debug support
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -DDEBUG
else
    CXXFLAGS += -O2 -DNDEBUG
endif

# ================================================
# Amber game engine link
# ================================================
AMB_DIR 		:= Amber
AMB_INC 		:= $(AMB_DIR)/inc
AMB_SRC 		:= $(AMB_DIR)/src
AMB_INC_EXT 	:= $(AMB_INC)/External
INCLUDES 		+= -I$(AMB_INC) -I$(AMB_INC_EXT)
ENGINE_NAME		:= Amber

LIBRARY 		+= -l$(ENGINE_NAME) -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lfreetype

# Gald
GLAD_SRC := $(AMB_SRC)/glad/glad.c
GLAD_OBJ := $(BUILD_DIR)/glad.o
GLAD_INC := -I $(AMB_INC)

# ================================================
# Automatic source detection (Windows friendly)
# ================================================

# Find all .cpp files recursively
SOURCES     := $(wildcard $(SRC_DIR)/*.cpp) \
               $(wildcard $(SRC_DIR)/**/*.cpp) \
               $(wildcard $(SRC_DIR)/**/**/*.cpp)

OBJECTS     := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDS     := $(OBJECTS:.o=.d)

# Main target
all: directories $(BIN_DIR)/$(TARGET)

directories:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)

glad:
	@echo "Building GLAD..."
	$(CXX) $(CXXFLAGS) -MMD -MP -c $(GLAD_SRC) -o $(GLAD_OBJ) $(GLAD_INC)

# Linking
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@echo Linking $@
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBRARY)

# Compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(dir $@) mkdir "$(dir $@)"
	@echo Compiling $<
	$(CXX) $(CXXFLAGS) $(LIBRARY) $(INCLUDES) -MMD -MP -c $< -o $@

# Include dependencies
-include $(DEPENDS)

# Phony targets
.PHONY: all clean run debug

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@if exist $(BIN_DIR)/$(TARGET) del /Q "$(BIN_DIR)\$(TARGET)"
	@echo Clean completed.

run: all
	@$(BIN_DIR)/$(TARGET)

debug:
	@$(MAKE) DEBUG=1 all

build_amber:
	@echo "=== Building Amber Engine ==="
	@$(MAKE) -C $(AMB_DIR)
	@$(MAKE) -C $(AMB_DIR) copy

help:
	@echo "Commands:"
	@echo "  make				- Build release"
	@echo "  make DEBUG=1		- Build with debug"
	@echo "  make run			- Build and run"
	@echo "  make clean			- Clean build files"
	@echo "  make build_amber	- Build Amber engine"