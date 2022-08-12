#pragma once
#include "BSP.h"

namespace Assignment
{
	namespace BSP
	{

		glm::vec3 Tree::GetSplitPlaneNormal(glm::vec3 pt0, glm::vec3 pt1, glm::vec3 pt2)
		{
			glm::vec3 vectorA = pt1 - pt0;
			glm::vec3 vectorB = pt2 - pt0;

			glm::vec3 vectorC = glm::cross(vectorA, vectorB);

			glm::vec3 normal = glm::cross(vectorA, -vectorC);
			return normal;
		}

		void Tree::Init(std::vector<TriangleA3*> triAdd)
		{
			

		}
	}

}