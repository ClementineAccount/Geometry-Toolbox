#include "CollisionHelper.h"
#include "AssignmentOne.h"

namespace AssignmentOne
{

	SphereCollider::SphereCollider()
	{
		colliderType = Collider::SPHERE;
		meshID = MeshNames::sphere;
	}

	void SphereCollider::UpdateModel()
	{
		model.scale.x = radius;
		model.scale.y = radius;
		model.scale.z = radius;
		model.pos = centerPos;
	}


	bool checkSphereOnSphere(SphereCollider const& lhs, SphereCollider const& rhs)
	{
		float radiusSquared = (lhs.radius + rhs.radius) * (lhs.radius + rhs.radius);
		float distanceSquared = glm::dot((lhs.centerPos - rhs.centerPos), (lhs.centerPos - rhs.centerPos));

		return glm::distance(lhs.centerPos, rhs.centerPos) < (lhs.radius + rhs.radius);
	}

	//Check if there is a collision between the lhs and rhs
	bool collisionCheck(CollisionObject const& lhs, CollisionObject const& rhs)
	{
		//Identify the type of collider lhs is

		//Is there a better way to check this?
		if (lhs.colliderType == Collider::SPHERE)
		{
			if (rhs.colliderType == Collider::SPHERE)
				return checkSphereOnSphere(static_cast<SphereCollider const&>(lhs), static_cast<SphereCollider const&>(rhs));
		}
		return false;
	}



}
