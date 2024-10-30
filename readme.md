    This Program CPP with OpenGL and GLFW, and the main part of the code is the loop that draws the triangles. based on tutorial https://www.youtube.com/watch?v=iyDJ_1lElms
    
    Key changes made to create the outlined triangle:
    - Changed the rendering mode from GL_TRIANGLES to GL_LINES
    - Modified the vertex data to define three separate lines instead of a filled triangle
    - Added glLineWidth(3.0f) to make the lines more visible
    - Updated the vertex positions and colors to create a continuous outline
    - Each line segment maintains the gradient effect between its endpoints
    
    The  positions  and  colors  vectors store all the vertices and colors for all the triangles. 

    Key changes made to create the layered effect:
    - Added multiple concentric triangles (5 by default)
    - Each inner triangle is scaled down by a factor of 0.15
    - Added color intensity variation - inner triangles have slightly lower color intensity
    - Used vectors to store the vertex and color data for all triangles
    - Enabled alpha blending for better visual effect
    - Adjusted line width to 2.0f for clearer visibility of all layers
    - Each triangle maintains the RGB gradient effect but with decreasing intensity
    
    The number of triangles drawn is controlled by the  NUM_TRIANGLES  constant. 
    
    The  scale  variable is used to calculate the size of each triangle. The  intensity  variable is used to calculate the intensity of the color based on the layer. 

    The  layerColors  array stores the colors for each triangle. The colors are interpolated from red to green, green to blue, and blue to red.
 
    The geometry shader takes a single triangle as input and generates three triangles as output. It copies the input triangle’s vertices and colors to the output triangles. 
    
    The  glDrawArrays(GL_LINES, 0, positions.size() / 2);  function draws all the triangles. The  GL_LINES  parameter tells OpenGL to draw the triangles as lines. 
    
    The  glLineWidth(2.0f);  function sets the line width to 2.0 pixels. 
    
    The  glEnable(GL_BLEND);  and  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  functions enable alpha blending. 
    
    The  glClearColor(0.02, 0.02, 0.02, 1.0);  function sets the clear color to a dark gray. 
    
    The  glfwWaitEvents();  function waits for events to occur. The  glfwSwapBuffers(window);  function swaps the buffers to display the drawing. 
    
    Compile and run the code: 
    g++ hello_triangle.cpp -o hello_triangle -lglfw -lGL -lGLEW
    ./hello_triangle
    
    The output should be a window with five layered triangles. Each triangle is smaller than the previous one, and the color intensity decreases from the outer triangles to the inner triangles. 

    Key Changed:
    - Replaced GLFW with native Wayland and EGL initialization
    - Created a WaylandEGLWindow class to manage Wayland and EGL setup/teardown
    - Updated shader version to "#version 300 es" for OpenGL ES 3.0
    - Added precision qualifier in fragment shader (required for GLES)
    - Removed GLEW dependency as it's not needed for GLES
    - Added proper error handling with exceptions
    - Added basic event processing with Wayland
    - Added frame timing with usleep for ~60 FPS
    - Added #include <string.h> for strcmp
    - Moved the static wl_registry_listener definition outside the class
    - Added proper initialization in the constructor using an initialization list
    - Marked unused parameters with (void) to suppress warnings
    - Added null checks in the destructor
    - Fixed the listener structure initialization using designated initializers
    
    Conclusion 
    In this tutorial, you learned how to create a simple OpenGL program to draw layered triangles using the modern OpenGL 4.1 API. You also learned how to set up a GLFW window and GLEW library to initialize the OpenGL context.

    Compile and Run
    setup your environtment to install
    - for using cpp, `sudo apt-get install clang++`
    - for using opengl es, `sudo apt-get install libglew-dev`
    - for using glew, `sudo apt-get install libglfw3 libglfw3-dev`
    - for using wayland, `sudo apt-get install libwayland-dev libgles2-mesa-dev libegl1-mesa-dev wayland-protocols`

    run command based on list of command in Makefile
    - compile, `make hello_triangle_(*type of API, Native API with Wayland EGL or Libary API with GLFW)`
    - run, `./hello_triangle_(*type)`
    - clean, `make clean_(*type)`