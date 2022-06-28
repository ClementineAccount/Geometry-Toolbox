#include "Assignment.h"

namespace Assignment
{
    //View matrix
    struct Camera
    {
        glm::vec3 pos{ defaultCameraPos };
        glm::vec3 targetPos{ worldOrigin }; //looking at the origin as the default
        glm::vec3 up{ worldUp };
        glm::vec3 right{ worldRight };

        float FOV{ defaultFOV };
    };
}
