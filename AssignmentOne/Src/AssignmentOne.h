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


namespace AssignmentOne
{
    constexpr GLuint defaultShaderID = 0;

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

    struct Vertex
    {
        glm::vec3 position;

        //Will add it later

        //glm::vec3 color;

        //glm::vec3 normal;
        //glm::vec2 texture;
    };

    struct Mesh
    {
        unsigned int VAO;
        unsigned int VBO;
    };


    //The origin for use in model and view projection
    constexpr glm::vec3 worldOrigin = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    constexpr glm::vec3 worldRight = { 1.0f, 0.0f, 0.0f };
    constexpr float defaultFOV = 90.0f;

    struct Model
    {
        glm::vec3 pos{ worldOrigin };
        glm::vec3 scale{1.0f, 1.0f, 1.0f};

        //in degrees for each axis
        glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
    };

    //View matrix
    struct Camera
    {
        glm::vec3 pos{ 1.0f, 1.0f, 1.0f };
        glm::vec3 targetPos{ worldOrigin }; //looking at the origin as the default
        glm::vec3 up{ worldUp };
        glm::vec3 right{ worldRight };

       float FOV{ defaultFOV };
    };



    //For testing some functions. I can just assign it here and then pass it in
    //Model placeholderModels[10];

    //void RenderModel(Model const& model, GLuint const shaderID = defaultShaderID);

    struct drawObject
    {
        Model const& model;
        Mesh const& mesh;
    };

    //Submit to the drawList
    void SubmitDraw(Model const& model, Mesh const& mesh);

    
    //Draws all 
    void DrawAll(std::vector<drawObject> const& drawList);

    struct AABB
    {
        bool isRendering;

        //For rendering
        Mesh mesh;
    };


    Mesh InitQuadMesh();
    int InitAssignment();

    void setApplicationPtr(GeometryToolbox::GLApplication& appPtr);

    void RenderDearImguiDefault();

    //Render with the mesh struct instead of more hardcoded approach
    void RenderQuadMesh();
    void RenderAssignment();

}


