#include <iostream>
#include <vector>
#include <string.h> // For strcmp
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <unistd.h>

class WaylandEGLWindow {
private:
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct wl_surface* surface;
    struct wl_egl_window* egl_window;
    struct wl_shell* shell;
    struct wl_shell_surface* shell_surface;
    
    EGLDisplay egl_display;
    EGLContext egl_context;
    EGLSurface egl_surface;
    
    bool running;
    
    static void registry_global(void* data, struct wl_registry* registry,
                              uint32_t name, const char* interface, uint32_t) {
        WaylandEGLWindow* window = static_cast<WaylandEGLWindow*>(data);
        
        if (strcmp(interface, "wl_compositor") == 0) {
            window->compositor = static_cast<struct wl_compositor*>(
                wl_registry_bind(registry, name, &wl_compositor_interface, 1));
        } else if (strcmp(interface, "wl_shell") == 0) {
            window->shell = static_cast<struct wl_shell*>(
                wl_registry_bind(registry, name, &wl_shell_interface, 1));
        }
    }
    
    static void registry_global_remove(void* data, struct wl_registry* registry, uint32_t name) {
        (void)data;  // Unused parameter
        (void)registry;  // Unused parameter
        (void)name;  // Unused parameter
    }

    // Define the listener structure outside the class
    static const struct wl_registry_listener registry_listener;

public:
    WaylandEGLWindow() : 
        display(nullptr),
        registry(nullptr),
        compositor(nullptr),
        surface(nullptr),
        egl_window(nullptr),
        shell(nullptr),
        shell_surface(nullptr),
        egl_display(EGL_NO_DISPLAY),
        egl_context(EGL_NO_CONTEXT),
        egl_surface(EGL_NO_SURFACE),
        running(true) {
            
        display = wl_display_connect(nullptr);
        if (!display) {
            throw std::runtime_error("Failed to connect to Wayland display");
        }

        registry = wl_display_get_registry(display);
        wl_registry_add_listener(registry, &registry_listener, this);
        wl_display_dispatch(display);
        wl_display_roundtrip(display);

        if (!compositor || !shell) {
            throw std::runtime_error("Failed to get Wayland compositor or shell");
        }

        surface = wl_compositor_create_surface(compositor);
        shell_surface = wl_shell_get_shell_surface(shell, surface);
        
        if (!shell_surface) {
            throw std::runtime_error("Failed to create shell surface");
        }

        wl_shell_surface_set_toplevel(shell_surface);
        
        egl_window = wl_egl_window_create(surface, 512, 512);
        if (!egl_window) {
            throw std::runtime_error("Failed to create EGL window");
        }

        // Initialize EGL
        egl_display = eglGetDisplay((EGLNativeDisplayType)display);
        if (egl_display == EGL_NO_DISPLAY) {
            throw std::runtime_error("Failed to get EGL display");
        }

        EGLint major, minor;
        if (!eglInitialize(egl_display, &major, &minor)) {
            throw std::runtime_error("Failed to initialize EGL");
        }

        const EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE
        };

        EGLConfig config;
        EGLint num_config;
        if (!eglChooseConfig(egl_display, config_attribs, &config, 1, &num_config)) {
            throw std::runtime_error("Failed to choose EGL config");
        }

        eglBindAPI(EGL_OPENGL_ES_API);

        const EGLint context_attribs[] = {
            EGL_CONTEXT_MAJOR_VERSION, 3,
            EGL_CONTEXT_MINOR_VERSION, 0,
            EGL_NONE
        };

        egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attribs);
        if (egl_context == EGL_NO_CONTEXT) {
            throw std::runtime_error("Failed to create EGL context");
        }

        egl_surface = eglCreateWindowSurface(egl_display, config, egl_window, nullptr);
        if (egl_surface == EGL_NO_SURFACE) {
            throw std::runtime_error("Failed to create EGL surface");
        }

        if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context)) {
            throw std::runtime_error("Failed to make EGL context current");
        }
    }

    ~WaylandEGLWindow() {
        if (egl_display != EGL_NO_DISPLAY) {
            eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (egl_surface != EGL_NO_SURFACE) {
                eglDestroySurface(egl_display, egl_surface);
            }
            if (egl_context != EGL_NO_CONTEXT) {
                eglDestroyContext(egl_display, egl_context);
            }
            eglTerminate(egl_display);
        }
        
        if (egl_window) wl_egl_window_destroy(egl_window);
        if (shell_surface) wl_shell_surface_destroy(shell_surface);
        if (surface) wl_surface_destroy(surface);
        if (shell) wl_shell_destroy(shell);
        if (compositor) wl_compositor_destroy(compositor);
        if (registry) wl_registry_destroy(registry);
        if (display) wl_display_disconnect(display);
    }

    void swapBuffers() {
        eglSwapBuffers(egl_display, egl_surface);
    }

    bool isRunning() const { return running; }
    
    void processEvents() {
        wl_display_dispatch_pending(display);
    }
};

// Define the static member outside the class
const struct wl_registry_listener WaylandEGLWindow::registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

int main() {
    try {
        WaylandEGLWindow window;

        // Create and compile shaders
        GLuint shader_program = glCreateProgram();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vert_source = R"(#version 300 es
            in vec2 position;
            in vec4 color;
            out vec4 color0;
            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
                color0 = color;
            })";

        const char* frag_source = R"(#version 300 es
            precision mediump float;
            out vec4 frag_color;
            in vec4 color0;
            void main() {
                frag_color = color0;
            })";

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

        // Create buffers
        enum {POSITION, COLOR, NUM_BUFFERS};
        GLuint vao, vbo[NUM_BUFFERS];

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(NUM_BUFFERS, vbo);

        std::vector<float> positions;
        std::vector<float> colors;

        const int NUM_TRIANGLES = 5;
        const float SCALE_FACTOR = 0.15f;

        for(int i = 0; i < NUM_TRIANGLES; i++) {
            float scale = 1.0f - (i * SCALE_FACTOR);
            
            float vertices[] = {
                -0.5f * scale, -0.5f * scale,
                0.0f * scale, 0.5f * scale,
                0.0f * scale, 0.5f * scale,
                0.5f * scale, -0.5f * scale,
                0.5f * scale, -0.5f * scale,
                -0.5f * scale, -0.5f * scale
            };
            
            positions.insert(positions.end(), std::begin(vertices), std::end(vertices));
            
            float intensity = 1.0f - (i * 0.15f);
            float layerColors[] = {
                1.0f * intensity, 0.0f, 0.0f, 1.0f,
                0.0f, 1.0f * intensity, 0.0f, 1.0f,
                0.0f, 1.0f * intensity, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f * intensity, 1.0f,
                0.0f, 0.0f, 1.0f * intensity, 1.0f,
                1.0f * intensity, 0.0f, 0.0f, 1.0f
            };
            
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

        glLineWidth(2.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);

        while(window.isRunning()) {
            window.processEvents();
            
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(shader_program);
            glBindVertexArray(vao);
            glDrawArrays(GL_LINES, 0, positions.size() / 2);
            
            window.swapBuffers();
            
            usleep(16667); // ~60 FPS
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}