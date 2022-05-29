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

	struct AABB : public CollisionObject
	{
		AABB() { colliderType = Collider::AABB; };
	};

	//Boudding Sphere for collision
	struct SphereCollider : public CollisionObject
	{
		SphereCollider();
		float radius;
		glm::vec3 centerPos;

		void UpdateModel();
	};

	bool collisionCheck(CollisionObject const& lhs, CollisionObject const& rhs);


	class CollisionHelper
	{

	};
}



