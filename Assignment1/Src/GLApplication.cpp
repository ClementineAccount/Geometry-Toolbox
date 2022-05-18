#pragma once

// Include standard headers
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Include Dear Imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

// Local / project headers
//#include "../Common/Scene.h"
#include "GLApplication.h"
#include "Scene.h"
#include "shader.hpp"
#include "SimpleScene_Quad.h"


// Function declarations
bool savePPMImageFile(std::string &filepath, std::vector<GLfloat> &pixels, int width, int height);

//////////////////////////////////////////////////////////////////////
GLFWwindow *window;
Scene  *currScene;

double gDeltaTime;
double gLastFrameTime;

//16:9 aspect ratio for testing (To Do: encapsulate this to ensure resizing is working properly
int gWindowWidth = 800;
int gWindowHeight = 600;

//Calculate and cache this if window size change
GLfloat gAspectRatio = 0.0f;


double GLApplication::getDeltaTime()
{
    return gDeltaTime;
}

int GLApplication::getWindowHeight()
{
    return gWindowHeight;
}

int GLApplication::getWindowWidth()
{
    return gWindowWidth;
}

float GLApplication::getAspectRatio()
{
    if (gAspectRatio == 0)
        gAspectRatio = static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);
    
    return gAspectRatio;
}



///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    // Setting up OpenGL properties
    glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(gWindowWidth, gWindowHeight, // window dimensions
                              "Sample 1 - Simple scene (Quad) with Scene Class", // window title
                              nullptr, // which monitor (if full-screen mode)
                              nullptr); // if sharing context with another window
    if (window == nullptr)
    {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible.\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
    // context but does NOT make the created context "current". We MUST make it current with the following
    // call
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // See https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp as reference
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    std::string glslVersion = "#version 410";
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());


    // Initialize the currScene
    currScene = new SimpleScene_Quad( gWindowWidth, gWindowHeight );

    // Scene::Init encapsulates setting up the geometry and the texture
    // information for the currScene
    currScene->Init();


    //Set initial values for the time calculations
    gDeltaTime = 0.0;
    gLastFrameTime = 0.0;

    //To Do possibly: refactor this into proper classes for use in all assignments here soon
    do
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Update the universal deltaTime between frames
        double currFrameTime = glfwGetTime();
        gDeltaTime = currFrameTime - gLastFrameTime;
        gLastFrameTime = currFrameTime;
        
        // Now render the currScene
        // Scene::Display method encapsulates pre-, render, and post- rendering operations
        currScene->Display();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();


    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

bool savePPMImageFile(std::string &filepath, std::vector<GLfloat> &pixels, int width, int height)
{
    std::ofstream  texFile(filepath);

    texFile << "P3" << std::endl;
    texFile << width << "  " << height << std::endl;
    texFile << "255" << std::endl;

    auto it = pixels.begin();

    for( int row = 0; row < height; ++row )
    {
        for (int col = 0; col < width; ++col)
        {
            texFile << *it++ << " ";
            texFile << *it++ << " ";
            texFile << *it++ << " ";
        }

        texFile << std::endl;
    }

    texFile.close();

    return true;
}
