#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

int main()
{
    if(!glfwInit()) std::cout << "GLFW init failed\n";
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    GLFWwindow *window = glfwCreateWindow(512, 512, "Layered Triangles 4.1", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    glewExperimental = true;
    if(glewInit() != GLEW_OK) std::cout << "GLEW init failed \n";
    
    unsigned int shader_program = glCreateProgram();
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vert_source = ""
    "#version 410\n"
    "in vec2 position;\n"
    "in vec4 color;\n"
    "out vec4 color0;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(position, 0.0, 1.0);\n"
        "color0 = color;\n"
    "}\n";

    const char* frag_source = ""
    "#version 410\n"
    "out vec4 frag_color;\n"
    "in vec4 color0;\n"
    "void main()\n"
    "{\n"
        "frag_color = color0;\n"
    "}\n";

    enum {POSITION_ATTRIB_LOC, COLOR_ATTRIB_LOC};

    glShaderSource(vs, 1, &vert_source, NULL);
    glShaderSource(fs, 1, &frag_source, NULL);

    glCompileShader(vs);
    glCompileShader(fs);

    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);

    glBindAttribLocation(shader_program, POSITION_ATTRIB_LOC, "position");
    glBindAttribLocation(shader_program, COLOR_ATTRIB_LOC, "color");

    glLinkProgram(shader_program);
    glValidateProgram(shader_program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    enum {POSITION, COLOR, NUM_BUFFERS};
    
    unsigned int vao, vbo[NUM_BUFFERS];

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); 

    glGenBuffers(NUM_BUFFERS, vbo);

    std::vector<float> positions;
    std::vector<float> colors;
    
    const int NUM_TRIANGLES = 5;  // total layered triangles
    const float SCALE_FACTOR = 0.15f;  // span each inner triangle shrinks
    
    for(int i = 0; i < NUM_TRIANGLES; i++) {
        float scale = 1.0f - (i * SCALE_FACTOR);
        
        // Base triangle vertices
        float vertices[] = {
            // First line
            -0.5f * scale, -0.5f * scale,
            0.0f * scale, 0.5f * scale,
            // Second line
            0.0f * scale, 0.5f * scale,
            0.5f * scale, -0.5f * scale,
            // Third line
            0.5f * scale, -0.5f * scale,
            -0.5f * scale, -0.5f * scale
        };
        
        // Add vertices to positions vector
        positions.insert(positions.end(), std::begin(vertices), std::end(vertices));
        
        // Calculate colors with varying intensity based on layer
        float intensity = 1.0f - (i * 0.15f);  // Decrease intensity for inner triangles
        float layerColors[] = {
            // First line (red to green)
            1.0f * intensity, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f * intensity, 0.0f, 1.0f,
            // Second line (green to blue)
            0.0f, 1.0f * intensity, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f * intensity, 1.0f,
            // Third line (blue to red)
            0.0f, 0.0f, 1.0f * intensity, 1.0f,
            1.0f * intensity, 0.0f, 0.0f, 1.0f
        };
        
        // Add colors to colors vector
        colors.insert(colors.end(), std::begin(layerColors), std::end(layerColors));
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_ATTRIB_LOC);
    glVertexAttribPointer(POSITION_ATTRIB_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[COLOR]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(COLOR_ATTRIB_LOC);
    glVertexAttribPointer(COLOR_ATTRIB_LOC, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);

    // Set line width for the outline
    glLineWidth(2.0f);
    
    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.02, 0.02, 0.02, 1.0);
    while(!glfwWindowShouldClose(window))
    {
        glfwWaitEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        // Draw all triangles
        glDrawArrays(GL_LINES, 0, positions.size() / 2);

        glfwSwapBuffers(window);
    }
}