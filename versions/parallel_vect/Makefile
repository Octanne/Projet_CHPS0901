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
# Potential flags : -std=c++11 -g -O3 -fno-math-errno -fno-trapping-math -march=native -funroll-loops
CXXFLAGS = -I$(HEADER_DIR) -Wall -Wextra -std=c++11 -g -flto=auto -O3 -march=native -funroll-loops -DNDEBUG -fno-math-errno -fno-trapping-math
LDFLAGS = -lGLEW -lGL -lglfw -fopenmp

# Targets
all: $(EXECUTABLE)

# No visualisation
novisual: LDFLAGS=-fopenmp
novisual: CXXFLAGS+=-DVISU=0
novisual: all

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
