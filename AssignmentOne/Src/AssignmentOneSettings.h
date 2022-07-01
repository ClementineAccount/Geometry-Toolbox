#pragma once
#include <glm/vec3.hpp>

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
}
