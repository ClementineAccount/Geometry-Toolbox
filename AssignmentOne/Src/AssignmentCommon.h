#pragma once
#include "GLApplication.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <unordered_map>

// Include GLFW
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "mesh.h"

namespace Assignment
{
    //The origin for use in model and view projection
    constexpr glm::vec3 worldOrigin = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    constexpr glm::vec3 worldRight = { 1.0f, 0.0f, 0.0f };
    constexpr glm::vec3 worldForward = { 0.0f, 0.0f, 1.0f };

    constexpr glm::vec3 unitLineBasisVector = worldRight;
    constexpr float unitLineScale = 1.0f;

    constexpr glm::vec3 defaultPivotPercent = { 0.5f, 0.5f, 0.0f };
    constexpr float defaultFOV = 90.0f;

    struct shaderFilePath
    {
        std::string vertexFilePath;
        std::string fragmentFilePath;
        std::string shaderName;
    };

    struct ShaderContainer
    {
    public:
        std::unordered_map<std::string, GLuint> shaderMap;

        //Loads a shader and adds that to the map. Wrapper around LoadShaders
        void loadShader(std::string const& shaderName, shaderFilePath const& shaderPath)
        {
            //To Do: Add asserts to make sure that the shader was added properly (no duplications and so on)
            GLuint programID = LoadShaders(shaderPath.vertexFilePath.c_str(), shaderPath.fragmentFilePath.c_str());
            shaderMap.insert({ shaderName, programID });
        }

        GLuint getShaderID(std::string const& shaderName)
        {
            return shaderMap.at(shaderName);
        }
    };

    struct Model
    {
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 pos{ worldOrigin };

        //in degrees for each axis
        glm::vec3 rotDegrees{ 0.0f, 0.0f, 0.0f };

        glm::vec3 pivotPercent{ defaultPivotPercent };

        //Can be inserted into the shader if wanted
        glm::vec3 color{ 1.0f, 1.0f, 1.0f };
    };

    struct drawCall
    {
        Model model;
        MeshBuffers const& mesh;

        GLuint shaderID = 0;

        unsigned int drawOrder = 0;
        bool isRendering = true; //idk if we should have this
    };

    using sceneFunction = std::function<void(void)>;
    struct AssignmentScene
    {
        std::function<void(void)> initScene;
        std::function<void(void)> updateScene;
        std::function<void(void)> renderScene;
    };



    struct Kinematics
    {
        float speed = 0.0f;
        glm::vec3 normVector = glm::vec3(0.0f, 0.0f, 0.0f);

        //Might implement later, would need to have all Kinematics update every frame
        //float acceleration = 0.0f;
        //glm::vec3 normAccelerationVector = glm::vec3(0.0f, 0.0f, 0.0f);
    };
}
