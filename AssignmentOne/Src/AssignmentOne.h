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
#include "AssignmentCommon.h"

//Forward declare
class CollisionObject;

namespace Assignment
{
    constexpr GLuint defaultShaderID = 0;


    struct Vertex
    {
        glm::vec3 position;

        //Will add it later

        //glm::vec3 color;

        //glm::vec3 normal;
        //glm::vec2 texture;
    };


    //Cheap and easy subsitition to allow user to rotate in a set order
    //struct AxisRotation
    //{
    //    float rotationDegrees = 0.0f;
    //    glm::vec3 rotationAxis = worldRight; //User must choose one of the three rotDegrees axis

    //};



    void UpdatePhysics(Model& model, Kinematics const& kinematics);


    constexpr glm::vec3 defaultCameraPos = { 5.0f, 5.0f, 3.0f };

    //View matrix
    struct Camera
    {
        glm::vec3 pos{ defaultCameraPos };
        glm::vec3 targetPos{ worldOrigin }; //looking at the origin as the default
        glm::vec3 up{ worldUp };
        glm::vec3 right{ worldRight };

       float FOV{ defaultFOV };
    };

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


