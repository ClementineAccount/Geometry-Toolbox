//Bounding Volume is a 'remake/port' of some of CollisionHelper.h functions to better support BVH and loaded objects
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "Transform.h"

namespace Assignment
{
	//Forward declare
	class Object;

	namespace BV
	{
		//base class interface
		class BoundingVolume
		{
			Transform model; //storing the transform within the AABB
			std::string meshID; //To Do: You could change this to use uint_32 instead eventually

			bool isActive = true;
			bool isRendering = true;
		};

		class AABB : BoundingVolume
		{
		public:
			glm::vec3 halfExtent;
			glm::vec3 centerPos;
			glm::vec3 scale; //Allows calculation of half extents

			//Updates the collision points and the model
			void CalculatePoints();
			void UpdateModel();

			glm::vec3 minPoint;
			glm::vec3 maxPoint;
		};


		std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList, size_t start, size_t end);
		std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList);

		void CalculateAABB(std::vector<glm::vec3>& positions, AABB& aabbRef);
	}
}


