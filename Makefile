CXX = clang++
CXXFLAGS = -std=c++17 -Wall -O2
SRC = hello_triangle.cpp
LIBS_GLFW = -lGLEW -lglfw -lGL
LIBS_OPENGLES = -lEGL -lGLESv2 -lwayland-client -lwayland-egl
OUTPUT = hello_triangle
SRC_WAYLAND = hello_triangle_wayland.cpp
SRC_GLFW = hello_triangle_glfw.cpp
OUTPUT_WAYLAND = triangle_wayland
OUTPUT_GLFW = triangle_glfw
CLEAN_WAYLAND = clean_wayland
CLEAN_GLFW = clean_glfw

$(OUTPUT_WAYLAND): $(SRC_WAYLAND)
	$(CXX) $(CXXFLAGS) $(SRC_WAYLAND) -o $(OUTPUT_WAYLAND) $(LIBS_OPENGLES)

$(OUTPUT_GLFW): $(SRC_GLFW)
	$(CXX) $(SRC_GLFW) $(LIBS_GLFW) -o $(OUTPUT_GLFW)

$(CLEAN_WAYLAND): $(OUTPUT_WAYLAND)
	rm -f $(OUTPUT_WAYLAND)

$(CLEAN_GLFW): $(OUTPUT_GLFW)
	rm -f $(OUTPUT_GLFW)