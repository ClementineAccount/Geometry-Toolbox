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

	AABB::AABB(glm::vec3 setPos, glm::vec3 setScale)
	{
		centerPos = setPos;
		scale = setScale;

		colliderType = Collider::AABB;
		meshID = MeshNames::cube; //use a 1x1x1 cube as the primitive for rendering
		CalculatePoints();
	}

	void AABB::CalculatePoints()
	{
		maxPoint = centerPos;

		maxPoint.x += scale.x * 0.5f;
		maxPoint.y += scale.y * 0.5f;
		maxPoint.z += scale.z * 0.5f;

		minPoint = centerPos;

		minPoint.x -= scale.x * 0.5f;
		minPoint.y -= scale.y * 0.5f;
		minPoint.z -= scale.z * 0.5f;
	}

	void AABB::UpdateModel()
	{
		model.scale = scale;
		model.pos = centerPos;
	}


	bool checkSphereOnSphere(SphereCollider const& lhs, SphereCollider const& rhs)
	{
		float radiusSquared = (lhs.radius + rhs.radius) * (lhs.radius + rhs.radius);
		float distanceSquared = glm::dot((lhs.centerPos - rhs.centerPos), (lhs.centerPos - rhs.centerPos));

		return glm::distance(lhs.centerPos, rhs.centerPos) < (lhs.radius + rhs.radius);
	}

	bool checkPointOnSphere(glm::vec3 const& point, SphereCollider const& sphere)
	{
		float radiusSquared = sphere.radius * sphere.radius;
		float distanceSquared = glm::dot((point - sphere.centerPos), (point - sphere.centerPos));

		return radiusSquared > distanceSquared;
	}

	bool checkPointOnAABB(glm::vec3 const& point, AABB const& aabb)
	{
		//Six boolean checks

		//Early rejection
		if ((point.x > aabb.maxPoint.x || point.x < aabb.minPoint.x) || 
			(point.y > aabb.maxPoint.y || point.y < aabb.minPoint.y) ||
			(point.z > aabb.maxPoint.z || point.z < aabb.minPoint.z))
			return false;

		return true;
	}

	bool checkAABBonAABB(AABB const& lhs, AABB const& rhs)
	{
		if (lhs.maxPoint.x < rhs.minPoint.x || rhs.maxPoint.x < lhs.minPoint.x ||
			lhs.maxPoint.y < rhs.minPoint.y || rhs.maxPoint.y < lhs.minPoint.y ||
			lhs.maxPoint.z < rhs.minPoint.z || rhs.maxPoint.z < lhs.minPoint.z)
			return false;

		return true;
	}

	bool checkAABBOnSphere(AABB const& aabb, SphereCollider const& sphere)
	{
		//Trivial acceptance test if both are at origin
		//if (aabb.centerPos == sphere.centerPos)
		//	return true;

		//Get the point on the sphere closest to the AABB
		glm::vec3 dir_to_center_AABB = glm::normalize(aabb.centerPos - sphere.centerPos);
		glm::vec3 nearestPointOnSphere = sphere.centerPos + (sphere.radius * dir_to_center_AABB);

		return checkPointOnAABB(nearestPointOnSphere, aabb);
	}


	//Check if there is a collision between the lhs and rhs
	bool collisionCheck(CollisionObject const& lhs, CollisionObject const& rhs, bool isSwap)
	{
		//Identify the type of collider lhs is

		if (lhs.colliderType == Collider::AABB)
		{
			if (rhs.colliderType == Collider::AABB)
				if (checkAABBonAABB(static_cast<AABB const&>(lhs), static_cast<AABB const&>(rhs)))
					return true;


			if (rhs.colliderType == Collider::SPHERE)
				if (checkAABBOnSphere(static_cast<AABB const&>(lhs), static_cast<SphereCollider const&>(rhs)))
					return true;
		}

		//Is there a better way to check this?
		if (lhs.colliderType == Collider::SPHERE)
		{
			if (rhs.colliderType == Collider::SPHERE)
				if (checkSphereOnSphere(static_cast<SphereCollider const&>(lhs), static_cast<SphereCollider const&>(rhs)))
					return true;
		}

		//Check one more time with a swap if false (to do: analyze the time complexity of this approach)
		if (!isSwap)
			collisionCheck(rhs, lhs, true);

		return false;
	}



}
