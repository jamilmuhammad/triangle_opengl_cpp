CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS_GLFW = -lGLEW -lglfw -lGL
LIBS_OPENGLES = -lEGL -lGLESv2 -lwayland-client -lwayland-egl

TARGET = triangle
SRC = hello_triangle.cpp

all: $(TARGET)

hello_triangle_%: $(SRC)
ifeq ($*,glfw)
	$(CXX) $(SRC) $(LIBS_GLFW) -o $@
else ifeq ($*,wayland)
	$(CXX) $(CXXFLAGS) $(SRC) -o $@ $(LIBS_OPENGLES)
endif
# $(TARGET): $(SRC)
# 	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS_OPENGLES)

clean:
	rm -f $(TARGET)

.PHONY: all clean