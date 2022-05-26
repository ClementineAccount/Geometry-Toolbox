#pragma once
#include "../Scene.h"
#include "../GLApplication.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

// Include GLFW
#include <GLFW/glfw3.h>

namespace Scenes
{
    namespace SceneQuad
    {
        struct QuadData : public SceneData
        {
            GeometryToolbox::GLApplication* parentApplication = nullptr;

            // data members
            GLuint  programID;
            GLuint  vertexbuffer;
            GLuint  VertexArrayID;

            std::vector<GLfloat>    geometryBuffer;
            GLfloat   angleOfRotation;

            float fieldOfView;
            glm::vec3 cameraPos;
            glm::vec3 cameraTargetPos;
            glm::vec3 quadPos;
            glm::vec3 quadScale;
            glm::vec3 quadAngles;

            glm::vec3 backgroundColor;
        };

       SceneClass CreateQuadScene();
       SceneClass CreateColorQuadScene();

       SceneClass::sceneFunctionReturnType Init(SceneClass::dataContainerType& dataCont, float deltaTime = 0.0f);
       SceneClass::sceneFunctionReturnType Render(SceneClass::dataContainerType& dataCont, float deltaTime = 0.0f);
    }


    
}
