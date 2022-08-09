#pragma once
#include "AssignmentOneSettings.h"

namespace Assignment
{

    struct Transform
    {
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 pos{ worldOrigin };

        //in degrees for each axis
        glm::vec3 rotDegrees{ 0.0f, 0.0f, 0.0f };

        glm::vec3 pivotPercent{ defaultPivotPercent };

        //Can be inserted into the shader if wanted
        glm::vec3 color{ 1.0f, 1.0f, 1.0f};
    };


}

