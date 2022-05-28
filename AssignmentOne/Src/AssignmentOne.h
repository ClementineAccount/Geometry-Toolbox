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
        unsigned int VAO = 0;
        unsigned int VBO = 0;

        unsigned int arrayCount;

        GLchar drawType = GL_TRIANGLES;
        bool isDrawElements = false;
        GLuint* indices = nullptr;
    };


    //The origin for use in model and view projection
    constexpr glm::vec3 worldOrigin = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    constexpr glm::vec3 worldRight = { 1.0f, 0.0f, 0.0f };
    constexpr glm::vec3 worldForward = { 0.0f, 0.0f, 1.0f };

    constexpr glm::vec3 defaultPivotPercent = { 0.5f, 0.5f, 0.0f };
    constexpr float defaultFOV = 90.0f;


    //Cheap and easy subsitition to allow user to rotate in a set order
    //struct AxisRotation
    //{
    //    float rotationDegrees = 0.0f;
    //    glm::vec3 rotationAxis = worldRight; //User must choose one of the three rotDegrees axis

    //};

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
        bool isRendering = true;
    };

    //Submit to the drawList
    void SubmitDraw(Model const& model, Mesh const& mesh);

    //Draws all 
    void DrawAll(std::vector<drawObject> const& drawList);

    Mesh InitQuadMesh(std::vector<GLfloat>& quadPositions, float quadScale = 0.5f);

    //I only wanna create meshes once so..
    void InitMeshes();

    int InitAssignment();

    void setApplicationPtr(GeometryToolbox::GLApplication& appPtr);

    void RenderDearImguiDefault();

    void UpdateAssignment();

    //Render with the mesh struct instead of more hardcoded approach
    void RenderQuadMesh();
    void RenderAssignment();

    //Functions that just add objects to scenes.  Gonna make it a class for now in case I want to make it a proper class in the future
    class ObjectMaker
    {
    public:
        static void MakeFloor();
    };

 
}


