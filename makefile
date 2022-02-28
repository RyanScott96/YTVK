CXX       := g++
CXX_FLAGS := --std=c++17 -g

BIN     := bin
SRC     := src
INCLUDE := include
SHADERS := shaders
LIBRARIES   := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
EXECUTABLE  := main

.PHONY: clean compile_shaders

all: clean compile_shaders $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	rm -rf $(BIN)/*
	rm -rf $(SHADERS)/*.spv

compile_shaders:
	./compile.sh