//Bounding Volume is a 'remake/port' of some of CollisionHelper.h functions to better support BVH and loaded objects
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <algorithm>

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
		public:
			Transform model; //storing the transform within the AABB
			std::string meshID; //To Do: You could change this to use uint_32 instead eventually

			bool isActive = true;
			bool isRendering = true;

			virtual glm::vec3 getCenter() const { return model.pos; };
		};

		class AABB : public BoundingVolume
		{
		public:
			glm::vec3 halfExtent;
			glm::vec3 centerPos;
			glm::vec3 scale; //Allows calculation of half extents

			glm::vec3 minPoint;
			glm::vec3 maxPoint;

			//Updates the collision points and the model
			void Update();

			glm::vec3 getCenter() const override { return centerPos; };
		};

		void CalculateAABB(std::vector<glm::vec3>& positions, AABB& aabbRef);


		struct objectSplitList
		{
			std::vector<Object*> allObjects;

			//Pointers to which objects are in the positive/negative half planes
			std::vector<Object*> positiveObjects;
			std::vector<Object*> negativeObjects;
		};

		//struct axisSplitHeuristic
		//{
		//	glm::vec3 splitAxis = worldRight;
		//	float axisOffset = 1.0f;
		//	float numAttempts = 10;
		//};


	    //Why is this a function? Because it allows passing this into SplitObjectRegions as that takes in functions
		glm::vec3 SplitPlaneAxis(glm::vec3 splitAxis = worldRight);

		glm::vec3 SplitPointMean(std::vector<Object*> const& objList);

		//void SplitObjectRegionsAxis(objectSplitList objList, axisSplitHeuristic heuristic);

		//Split the object into the positive and negative half planes with the split func as the herustic chosen
		void SplitObjectRegions(objectSplitList objList, glm::vec3 splitPlanePoint, glm::vec3 splitPlaneDir);
		void SplitObjectRegions(std::vector<Object*>& objList, glm::vec3 splitPlanePoint, glm::vec3 splitPlaneDir);
	}
}


