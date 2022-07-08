#include "Assignment.h"
#include "GLApplication.h"

namespace Assignment
{

    //https://learnopengl.com/Getting-started/Camera
    //View matrix
    struct Camera
    {
        glm::vec3 pos{ defaultCameraPos };
        glm::vec3 targetPos{ worldOrigin }; //looking at the origin as the default
        glm::vec3 up{ worldUp };
        glm::vec3 right{ worldRight };
        glm::vec3 forward{ worldForward };
        glm::vec3 targetVector;

        float FOV{ defaultFOV };

        float yaw = 0.0f;
        float pitch = 0.0f;
        int lastX;
        int lastY;

        float offsetX = 0.0f;
        float offsetY = 0.0f;

        bool firstMouse = true;
        float sens = 0.002f;

        float cameraSpeed = 2.0f;

        void updateCamera(GeometryToolbox::GLApplication const* app);

        void updateCameraMovement(GeometryToolbox::GLApplication const* app);

        void updateCameraVectors();
        //glm::mat4 getViewMatrix();


    private:

    };
}
