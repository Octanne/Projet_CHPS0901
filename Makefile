# Makefile for C++ project
INSTALL_DIR=libs/install

# Compiler
CXX = mpicxx

# Directories
SRC_DIR = sources
HEADER_DIR = headers
OBJ_DIR = obj
BIN_DIR = bin

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(HEADER_DIR)/*.h)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
EXECUTABLE = $(BIN_DIR)/main

# Flags
CXXFLAGS = -I$(HEADER_DIR) -Wall -Wextra -std=c++11 -g -O3
LDFLAGS = -lGLEW -lGL -lglfw -lpthread -fopenmp
LDFLAGS_LOCAL = -I${INSTALL_DIR}/include -L${INSTALL_DIR}/lib64 -lGLEW -lglfw -lGL -lpthread -fopenmp

# Targets
all: $(EXECUTABLE)

# Local library build
locallibs: LDFLAGS=$(LDFLAGS_LOCAL)
locallibs: CXXFLAGS+=-I${INSTALL_DIR}/include
locallibs: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
