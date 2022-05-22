//
// Created by pushpak on 6/1/18.
//



#include "GLApplication.h"
#include "SimpleScene_Quad.h"
#include "shader.hpp"
//#include <shader.hpp>
#include <glm/vec3.hpp>


// Include Dear Imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


//https://stackoverflow.com/questions/61316071/no-file-type-mat-hpp-in-glm-detail
//#include <glm/detail/type_mat.hpp>

#include <glm/glm.hpp>

//For GLApplication
using namespace GeometryToolbox;

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::SetupNanoGUI(GLFWwindow *pWindow)
{
    pWindow = nullptr;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::~SimpleScene_Quad()
{
    initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::SimpleScene_Quad(int windowWidth, int windowHeight, GeometryToolbox::GLApplication* parentApplicationPtr) : Scene(windowWidth, windowHeight, parentApplicationPtr),
                                                                        programID(0), vertexbuffer(0), VertexArrayID(0),
                                                                        angleOfRotation(0.0f)
{
    this->parentApplicationPtr = parentApplicationPtr;
    initMembers();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-unused-return-value"
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::initMembers()
{
    programID = 0;
    vertexbuffer = 0;
    VertexArrayID = 0;

    geometryBuffer.empty();
    angleOfRotation = 0.0f;
}
#pragma clang diagnostic pop

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::CleanUp()
{
    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::SetupBuffers()
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, geometryBuffer.size() * sizeof(GLfloat),
                 geometryBuffer.data(), GL_STATIC_DRAW);

    return;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Init()
{
    // Create and compile our GLSL program from the shaders

    //To Do: Read from filepath from another config file
    programID = LoadShaders("../Common/Shaders/QuadVertexShader.vert",
                            "../Common/Shaders/QuadFragmentShader.frag");

    // Just two triangles making up a quad
    geometryBuffer = {  0.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,
                        1.0f, 1.0f, 0.0f,
                        0.0f, 1.0f, 0.0f
    };

    cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    cameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f); //origin start

    fieldOfView = 90.0f;

    quadScale = glm::vec3(1.0f, 1.0f, 1.0f);
    quadAngles = glm::vec3(0.0f, 0.0f, 0.0f);
    quadPos = glm::vec3(0.0f, 0.0f, -3.0f);

    backgroundColor = glm::vec3(0.0f, 0.0f, 0.7f);

    SetupBuffers();

    return Scene::Init();
}


int SimpleScene_Quad::preRender()
{
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Settings");
        ImGui::DragFloat3("Camera Position", (float*) &cameraPos, 0.01f, -10.0f, 10.0f);
        ImGui::DragFloat3("Camera Target Position", (float*)&cameraTargetPos, 0.01f, -10.0f, 10.0f);
        ImGui::DragFloat("FOV", &fieldOfView, 0.01f, 1.0f, 110.0f);
        ImGui::DragFloat3("Quad-Plane Position", (float*)&quadPos, 0.01f, -20.0f, 20.0f);
        ImGui::DragFloat3("Quad-Plane Scale", (float*)&quadScale, 0.01f, -20.0f, 20.0f);
        ImGui::DragFloat3("Quad-Plane Rotation (Degrees)", (float*)&quadAngles, 0.01f, -360.0f, 360.0f);
        ImGui::DragFloat3("Background Color", (float*)&backgroundColor, 0.001f, 0.0f, 1.0f);

        ImGui::End();
    }
    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Render()
{
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glUseProgram(programID);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer( 0, 3,GL_FLOAT, GL_FALSE, 0,(void *) 0 );

    // Uniform matrix
    // Uniform transformation (vertex shader)
    GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");

    // Draw the triangle !
    // T * R * S * Vertex
    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::vec3 scaleVector = glm::vec3(1.0f);
    glm::vec3 centroid = glm::vec3( 0.0f, -0.0f, 0.0f );
    glm::vec3 triPos = glm::vec3(0.0f, -0.5f, -1.0f);

    modelMat = glm::translate(modelMat, quadPos);

    float pivotPercentFromLeft = 0.5f;

    modelMat = glm::translate(modelMat, glm::vec3(pivotPercentFromLeft * quadScale.x, pivotPercentFromLeft * quadScale.y, pivotPercentFromLeft * quadScale.z));
    
    modelMat = glm::rotate(modelMat, quadAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::rotate(modelMat, quadAngles.y, glm::vec3(0.0f, 1.0f, 1.0f));
    modelMat = glm::rotate(modelMat, quadAngles.x, glm::vec3(1.0f, 0.0f, 1.0f));
    modelMat = glm::translate(modelMat, glm::vec3(-pivotPercentFromLeft * quadScale.x, -pivotPercentFromLeft * quadScale.y, -pivotPercentFromLeft * quadScale.z));
    
    modelMat = glm::scale(modelMat, quadScale);


    // Prototype perspective projection (to do: refactor this out later)
    glm::mat4 perspectiveMat = glm::mat4(1.0f);
    GLfloat fov = 45.0f;
    GLfloat aspectRatio = static_cast<GLfloat>(parentApplicationPtr->getAspectRatio());
    GLfloat nearPlanePoint = 0.1f;
    GLfloat farPlanePoint = 100.0f;

    perspectiveMat = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlanePoint, farPlanePoint);
 

    //perspectiveMat = glm::ortho(0.0f, 800.0f,
    //    0.0f, 600.0f, 0.1f, 100.0f);



    //perspectiveMat = glm::mat4(1.0f);
    glm::mat4 viewMat = glm::mat4(1.0f);

    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    viewMat = glm::lookAt(cameraPos, cameraTargetPos, upVector);


    glm::mat4 mvpMat = glm::mat4(1.0f);
    mvpMat = perspectiveMat * viewMat * modelMat;
    
    // To Do: Calculate and inject the view matrix into here before we pass it to the shader


    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMat[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::postRender()
{
    angleOfRotation += 0.01f * parentApplicationPtr->getDeltaTime();
    return 0;
}
