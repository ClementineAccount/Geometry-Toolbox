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
SimpleScene_Quad::SimpleScene_Quad(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
                                                                        programID(0), vertexbuffer(0), VertexArrayID(0),
                                                                        angleOfRotation(0.0f)
{
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



    cameraY = 0.0f;

    SetupBuffers();

    return Scene::Init();
}


int SimpleScene_Quad::preRender()
{
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        ImGui::DragFloat("Camera pos y", &cameraY, 0.01f, -10.0f, 10.0f);
        ImGui::End();
    }
    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Render()
{
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

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

    modelMat = glm::translate(modelMat, triPos);
    modelMat = glm::rotate(modelMat, angleOfRotation, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMat = glm::scale(modelMat, scaleVector);


    // Prototype perspective projection (to do: refactor this out later)
    glm::mat4 perspectiveMat = glm::mat4(1.0f);
    GLfloat fov = 45.0f;
    GLfloat aspectRatio = static_cast<GLfloat>(GLApplication::getAspectRatio());
    GLfloat nearPlanePoint = 0.1f;
    GLfloat farPlanePoint = 100.0f;

    perspectiveMat = glm::perspective(glm::radians(fov), aspectRatio, nearPlanePoint, farPlanePoint);
 

    //perspectiveMat = glm::ortho(0.0f, 800.0f,
    //    0.0f, 600.0f, 0.1f, 100.0f);



    //perspectiveMat = glm::mat4(1.0f);
    glm::mat4 viewMat = glm::mat4(1.0f);

    glm::vec3 cameraPos = glm::vec3(0.0f, cameraY, 3.0f);
    glm::vec3 cameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
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
    angleOfRotation += 0.01f * GLApplication::getDeltaTime();
    return 0;
}
