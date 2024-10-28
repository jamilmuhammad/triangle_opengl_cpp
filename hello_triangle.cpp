#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main()
{
    if(!glfwInit()) std::cout << "GLFW init failed\n";
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);


    GLFWwindow *window = glfwCreateWindow(512, 512, "Hellow Triangle 4.1", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    glewExperimental = true;
    if(glewInit() != GLEW_OK) std::cout << "GLEW init failed \n";
    
    unsigned int shader_program = glCreateProgram();
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

    // vertex source
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

    // fragment source
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

    const float positions[] = {
        -0.5, -0.5,
        0.0, 0.5,
        0.5, -0.5
    };

    const float colors[] = {
        1, 0, 0, 1,
        0, 1, 0, 1,
        0, 0, 1, 1
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[POSITION]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_ATTRIB_LOC);
    glVertexAttribPointer(POSITION_ATTRIB_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[COLOR]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(COLOR_ATTRIB_LOC);
    glVertexAttribPointer(COLOR_ATTRIB_LOC, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);

    glEnable(GL_FRAMEBUFFER_SRGB);
    
    glClearColor(0.02,0.02,0.02,1.0);
    while(!glfwWindowShouldClose(window))
    {
        glfwWaitEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
    }
}
