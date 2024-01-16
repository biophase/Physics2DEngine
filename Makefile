SRC_DIR = src
BUILD_DIR = build/debug
CC = g++
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_NAME = play
# headers
INCLUDE_PATHS = -I include/SDL2/ -I include/SDL2_image/ -I include/Eigen
# library paths
LIBRARY_PATHS = -L lib/SDL2/ -L lib/SDL2_image
# c++ version, all warnings, no optimization, enable debuging
COMPILER_FLAGS = -std=c++17 -Wall -O0 -g
# linker flags, which will have the link to the SDL2 library --> not the same as library paths??
LINKER_FLAGS = -l sdl2

all:
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SRC_FILES) -o $(BUILD_DIR)/$(OBJ_NAME)

run:
	$(BUILD_DIR)/$(OBJ_NAME)