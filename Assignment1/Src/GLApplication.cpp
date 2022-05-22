


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


int GeometryToolbox::GLApplication::initApplication()
{
    return 0;
}

int main()
{
    GeometryToolbox::GLApplication app;

    //app.initApplication();
}
