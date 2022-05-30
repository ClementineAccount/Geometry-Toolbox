#pragma once
#include "AssignmentOne.h"
#include <memory>

namespace AssignmentOne
{
	

	
	enum class Collider
	{
		AABB,
		SPHERE
	};


	//Polymorphism so we can store it in the same container
	struct CollisionObject
	{
		Collider colliderType;

		Model model; //storing the transform within the AABB
		std::string meshID; //To Do: You could change this to use uint_32 instead eventually
		bool isActive = true;
	};


	constexpr glm::vec3 defaultPosAABB = worldOrigin;
	constexpr glm::vec3 defaultScaleAABB = glm::vec3(1.0f, 1.0f, 1.0f);


	//Naive implementation using min and max first (update to use half extents if have time)
	struct AABB : public CollisionObject
	{
		AABB(glm::vec3 setPos = defaultPosAABB, glm::vec3 setScale = defaultScaleAABB);

		glm::vec3 centerPos;
		glm::vec3 scale; //Allows calculation of half extents

		//Updates the collision points and the model
		void CalculatePoints();
		void UpdateModel();

		glm::vec3 minPoint;
		glm::vec3 maxPoint;
	};

	//Boudding Sphere for collision
	struct SphereCollider : public CollisionObject
	{
		SphereCollider();
		float radius;
		glm::vec3 centerPos;

		void UpdateModel();
	};

	bool collisionCheck(CollisionObject const& lhs, CollisionObject const& rhs, bool isSwap = false);

	bool checkPointOnAABB(glm::vec3 const& point, AABB const& aabb);

	bool checkPointOnSphere(glm::vec3 const& point, SphereCollider const& sphere);


	class CollisionHelper
	{

	};
}



