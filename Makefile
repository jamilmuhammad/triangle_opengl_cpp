CXX = clang++
SRC = hello_triangle.cpp
LIBS = -lGLEW -lglfw -lGL
OUTPUT = hello_triangle

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CXX) $(SRC) $(LIBS) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)