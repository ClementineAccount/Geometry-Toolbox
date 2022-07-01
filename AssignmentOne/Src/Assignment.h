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


#include "Transform.h"
#include "AssignmentOneSettings.h"

#include "CollisionHelper.h"

namespace Assignment
{
    //Forward declare
    class CollisionObject;
    class Camera;
    class Object;

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


    struct Kinematics
    {
        float speed = 0.0f;
        glm::vec3 normVector = glm::vec3(0.0f, 0.0f, 0.0f);


        //Might implement later, would need to have all Kinematics update every frame
        //float acceleration = 0.0f;
        //glm::vec3 normAccelerationVector = glm::vec3(0.0f, 0.0f, 0.0f);
    };

    constexpr glm::vec3 defaultCameraPos = { 5.0f, 5.0f, 3.0f };

    //Viewports can be treated like 2D plane model transformations
    struct ViewportModel
    {
        glm::vec2 bottomLeft{ 0.0f, 0.0f };
        glm::vec2 viewportScale{ 1.0f, 1.0f }; //in pixels
    };

    struct drawCall
    {
        Transform model;
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

    //To Do: Switch to using enums as the key instead of strings
    static std::unordered_map<std::string, MeshBuffers> meshMap;


    //Submit to the drawList
    //void SubmitDraw(std::string const modelName, std::string const meshName, std::string const shaderName = defaultShader.shaderName);

    //Draws all 
    void DrawAll(std::vector<drawCall>& drawList, Camera const& drawCamera);

    MeshBuffers InitQuadMesh(std::vector<GLfloat>& quadPositions, float quadScale = 0.5f);

    //I only wanna create meshes once so..
    void InitMeshes();

    int InitAssignment();

    void setApplicationPtr(GeometryToolbox::GLApplication& appPtr);

    void RenderDearImguiDefault();

    void UpdateAssignment();

    //Render with the mesh struct instead of more hardcoded approach
    void RenderQuadMesh();
    void RenderAssignment();


    //To Do: Use enum instead of string in the future 
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

    //enum class MeshID
    //{
    //    CUBE
    //};

    //static std::unordered_map<MeshID, MeshBuffers> meshMap;

    //'Scenes' that are just a collection of functions. Took away some abstraction from a more overengineered implementation
    namespace SceneNames
    {
        const char defaultScene[] = "Default (Blank)";

        //Just the axis and cube
        const char TestSceneCube[] = "Cube Primitive Example";

        //Just the axis and sphere
        const char TestSceneSphere[] = "Sphere Primitive Example";

        //Assignment One
        const char SphereVsSphere[] = "(01) Sphere Vs Sphere";
        const char AABBVsSphere[] = "(02) AABB Vs Sphere";
        const char SphereVsAABB[] = "(03) Sphere vs AABB";
        const char AABBVsAABB[] = "(04) AABB vs AABB";
        const char PointVsSphere[] = "(05) Point vs Sphere";
        const char PointVsAABB[] = "(06) Point vs AABB";
        const char PointVsPlane[] = "(08) Point vs Plane";
        const char RayVsPlane[] = "(09) Ray vs Plane";
        const char RayVsAABB[] = "(10) Ray Vs AABB";
        const char RayVsSphere[] = "(11) Ray Vs Sphere";
        const char PlaneVsAABB[] = "(12) Plane vs AABB";
        const char PlaneVsSphere[] = "(13) Plane vs Sphere";
        const char PointVsTriangle[] = "(07 + 12) Point and Ray vs Triangle";


        const char CubeSceneTest[] = "CubeSceneTest";
        const char AssignmentTwo[] = "Assignment Two";
        const char AssimapExample[] = "AssimapExample";
    }

    //Load the scene from a text file
    void LoadScene(std::vector<Object>& objectVectorRef, std::string const& scenePath);


}


