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

//Forward declare
class CollisionObject;
class Camera;

namespace Assignment
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

    struct MeshPrimitives
    {
        unsigned int VAO = 0;
        unsigned int VBO = 0;

        unsigned int arrayCount;

        GLchar drawType = GL_TRIANGLES;

        bool isDrawElements = false;
        unsigned int elementCount;
        unsigned int EBO;
    };


    //The origin for use in model and view projection
    constexpr glm::vec3 worldOrigin = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    constexpr glm::vec3 worldRight = { 1.0f, 0.0f, 0.0f };
    constexpr glm::vec3 worldForward = { 0.0f, 0.0f, 1.0f };

    constexpr glm::vec3 unitLineBasisVector = worldRight;
    constexpr float unitLineScale = 1.0f;

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

    struct Kinematics
    {
        float speed = 0.0f;
        glm::vec3 normVector = glm::vec3(0.0f, 0.0f, 0.0f);


        //Might implement later, would need to have all Kinematics update every frame
        //float acceleration = 0.0f;
        //glm::vec3 normAccelerationVector = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    void UpdatePhysics(Model& model, Kinematics const& kinematics);


    constexpr glm::vec3 defaultCameraPos = { 5.0f, 5.0f, 3.0f };



    //Viewports can be treated like 2D plane model transformations
    struct ViewportModel
    {
        glm::vec2 bottomLeft{ 0.0f, 0.0f };
        glm::vec2 viewportScale{ 1.0f, 1.0f }; //in pixels
    };

    //For testing some functions. I can just assign it here and then pass it in
    //Model placeholderModels[10];

    //void RenderModel(Model const& model, GLuint const shaderID = defaultShaderID);


    //Bad 
    struct GameObject
    {
        Model model; //the transforms
        std::string meshID; //To Do: You could change this to use uint_32 instead eventually
        bool isActive = true;
        
        std::unique_ptr<CollisionObject> colliderPtr;
        std::unique_ptr<Kinematics> kinematics = std::make_unique<Kinematics>();
    };

    struct drawCall
    {
        Model model;
        MeshPrimitives const& mesh;

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

    static std::unordered_map<std::string, MeshPrimitives> meshMap;


    //Submit to the drawList
    //void SubmitDraw(std::string const modelName, std::string const meshName, std::string const shaderName = defaultShader.shaderName);

    //Draws all 
    void DrawAll(std::vector<drawCall>& drawList, Camera const& drawCamera);

    MeshPrimitives InitQuadMesh(std::vector<GLfloat>& quadPositions, float quadScale = 0.5f);

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


    namespace MeshNames
    {
        //can be used for planes too
        static const char quad[] = "quad";
        static const char axis[] = "axis";
        static const char axisInverted[] = "axisInverted";
        static const char point[] = "point";
        static const char worldLine[] = "worldLine";

        static const char quadNormalForward[] = "quadFoward";
        static const char quadNormalUp[] = "quadUp";
        static const char quadNormalRight[] = "quadRight";
        
        static const char cube[] = "cube";
        static const char ring[] = "ring";
        static const char sphere[] = "sphere";

        static const char rayForward[] = "rayForward";
        static const char rayUp[] = "rayUp";
        static const char rayRight[] = "rayRight";
    }
 
}


