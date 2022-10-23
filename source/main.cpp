/*
Roughness-Based Heightmap Tessellation.

Based on the excellent LearnOpenGL guest articles Dr. Jeffrey Paone.
https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map
https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation

23/10/2023
Patrick Hume
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>

#include <iostream>

#include "../headers/heightmap.h"
#include "../headers/camera.h"
#include "../headers/screen.h"

// Called when the window is resized, resizes the viewport to match.
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
// A function for keeping track of whether a key is pressed
// as opposed to being held over a series of frames.
bool firstPress(GLFWwindow *window, int key);
// Toggled by pressing the P key when the mouse has focused on the window.
// When true, the scene will be rendered as polygons.
bool viewingPolygons = false;
// An array of bools representing the state of each key.
// Used in the firstPress.
bool keyIsHeld[260] = { 0 };

int main(int argc, char** argv)
{
    // The name of the heightmap file in resources/heightmaps (not including file extension).
    std::string filename;
    // Input handling:
    if(argc == 2){
        filename = "resources/heightmaps/" + std::string(argv[1]) + ".png";
    }else{
        std::cout << "Error: Invalid number of args." << std::endl;
        std::cout << "(Hint: Try "+std::string(argv[0])+" alps)" << std::endl;
        exit(0);
    }

    // Initialize and configure GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create a GLFW window.
    GLFWwindow* window = glfwCreateWindow(Screen::windowWidth, Screen::windowHeight, "multiplayer-game", NULL, NULL);
    if (window == NULL)
    {// Window creation error handling.
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }
    // Set OpenGl current contex to this window.
    glfwMakeContextCurrent(window);
    // Resize the viewport on window resize.
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Load OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    // Bind the frame buffer to 0, the frame that is drawn to the viewport.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Enable depth and stencil testing.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // Only overwrite the pixel if both depth and stencil tests pass.
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Record the window dimensions in the screen class.
    Screen::recordDimensions(window);

    // Create the heightmap and load the supplied file.
    Heightmap heightmap = Heightmap(filename.c_str());

    // Create the camera and set some default values.
    Camera camera;
    camera.setPosition(glm::vec3(0.0f, 10.0f, 0.0f));
    camera.setPerspective(45.0f, 0.1f, 10000.0f);
    
    // Run the mainloop until the window is closed by the user.
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // If L key is pressed: toggle wireframe view.
        if (firstPress(window, GLFW_KEY_P) == GLFW_PRESS && camera.focus && !camera.locked){
            viewingPolygons = !viewingPolygons;
            if(viewingPolygons){
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else{
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        // Pass the user input to world.
        camera.Inputs(window);
        // Update the physics of all objects in world.
        camera.updateMatrix();
        // Render the world to the back buffer.

        // Clear the viewport with a solid colour.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // Reset the buffers.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // Draw the heightmap.
        heightmap.Draw(camera);

		// Swap the back buffer with the front buffer and display to the viewport.
		glfwSwapBuffers(window);
		// Poll all GLFW events.
		glfwPollEvents();
    }

    // Delete the window before ending the program.
	glfwDestroyWindow(window);
    // Clear all previously allocated GLFW resources.
    glfwTerminate();

    return 0;
}

// Resize the viewport on window resize.
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Record the new window dimensions in Screen and resize.
    Screen::updateAndResize(window, width, height);
}

// Records whether a key has just been pressed (true), or is being held over several frames (false).
bool firstPress(GLFWwindow *window, int key){
    // 0 to 260 is the range of keycodes being recorded.
    if (key >= 260){
        // If out of range, return false.
        return false;
    }
    // Otherwise check if already held.
    if (glfwGetKey(window, key) == GLFW_PRESS){
        if(!keyIsHeld[key]){
            // If not, set state to held and return true.
            keyIsHeld[key] = true;
            return true;
        }
    }else{
        keyIsHeld[key] = false;
    }
    // Otherwise, return false.
    return false;
}