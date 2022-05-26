


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

#include "GLApplication.h"
#include "Scene.h"
#include "shader.hpp"
#include "Scenes/SimpleScene_Quad.h"

#include "AssignmentOne.h"


double GeometryToolbox::GLApplication::getDeltaTime()
{
    return gDeltaTime;
}

int GeometryToolbox::GLApplication::getWindowHeight()
{
    return gWindowHeight;
}

int GeometryToolbox::GLApplication::getWindowWidth()
{
    return gWindowWidth;
}

float GeometryToolbox::GLApplication::getAspectRatio()
{
    if (gAspectRatio == 0)
        gAspectRatio = static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight);

    return gAspectRatio;
}

int GeometryToolbox::GLApplication::shutdownApplication()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}

int GeometryToolbox::GLApplication::updateApplication()
{
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Update the universal deltaTime between frames
        double currFrameTime = glfwGetTime();
        gDeltaTime = currFrameTime - gLastFrameTime;
        gLastFrameTime = currFrameTime;

        //Need to have sceneManager update here
        //Scenes::SceneFunctions::LoopFunctions(scenePtr.get()->runtimeFunctions, scenePtr.get()->sceneDataContainer, 0.0f);

        //sm.runScenes(gDeltaTime);

        for (auto const& updateFunc : updateFunctions)
        {
            updateFunc();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

int GeometryToolbox::GLApplication::initApplication()
{
    gWindowWidth = 800;
    gWindowHeight = 600;
    gAspectRatio = 0.0f;

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

    //Set initial values for the time calculations
    gDeltaTime = 0.0;
    gLastFrameTime = 0.0;

    return 0;
}


