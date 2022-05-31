#include "CollisionHelper.h"
#include "AssignmentOne.h"

#include <glm/gtx/norm.hpp>

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

	float distanceFromPlane(glm::vec3 const& point, Plane const& plane)
	{
		glm::vec3 vectorToPoint = point - plane.pointOnPlane;
		return glm::dot(vectorToPoint, plane.outwardNormal);
	}

	bool checkPointOnPlane(glm::vec3 const& point, Plane const& plane, float eplsion)
	{
		auto isCloseToZero = [eplsion](float lhs, float rhs)
		{
			return fabs(lhs - rhs) <= eplsion;
		};

		//Check distance from the plane is equal to zero.

		float nearestDistanceFromPlane = distanceFromPlane(point, plane);

		return isCloseToZero(nearestDistanceFromPlane, 0.0f);
	}


	glm::vec3 Ray::getPointAtHere(float const t) const
	{
		//Normalize the direction if haven't already done so.
		return (startPoint + (t * glm::normalize(direction)));
	}

	float getIntersectionTimeRayOnPlane(Ray const& ray, Plane const& plane)
	{
		//Assumption: outwardNormal and rayDirection are not parallel
		if (glm::dot(plane.outwardNormal, ray.direction) == 0)
			return 0.0f; //It intersects at the start point and it also intersects anywhere as the plane is parallel

		//Solve for t intersection : Ray(t) = point on plane --> ray(t) = startPt + t(DirectionVector)
		glm::vec3 anyVectorToRay = ray.startPoint - plane.pointOnPlane;
	
		//Note: this result can return negative values if so desired.
		float t_before_length = glm::dot(-plane.outwardNormal, anyVectorToRay) / glm::dot(plane.outwardNormal, ray.direction);

		return glm::dot(-plane.outwardNormal, anyVectorToRay) / glm::dot(plane.outwardNormal, ray.direction);
	}


	bool checkRayOnPlaneGoingOutwards(Ray const& ray, Plane const& plane)
	{
		//Create a similar plane that has an inverted normal
		Plane flippedPlane;
		flippedPlane.pointOnPlane = plane.pointOnPlane;
		flippedPlane.outwardNormal = -plane.outwardNormal; 

		return checkRayOnPlane(ray, flippedPlane);
	}

	//Note, currently only supports outward intersecting to inwards plane. 
	bool checkRayOnPlane(Ray const& ray, Plane const& plane)
	{
		//Trivial rejection test: ray is facing away from plane (facing towards plane normal)
		float dotNormalDir = glm::dot(ray.direction, plane.outwardNormal);
		if (dotNormalDir > 0.0f)
		{
			//Ray is either approaching away from plane
			return false;
		}

		//Plane is parallel to ray. We can check if ray is on the plane via the start point
		if (dotNormalDir == 0.0f)
		{
			//Check if the start point is on the plane already if parallel.
			return (checkPointOnPlane(ray.startPoint, plane));
		}


		//Reject if the ray is too short. Clamp the t relative to the length
		return ((getIntersectionTimeRayOnPlane(ray, plane) / ray.length) <= 1.0f);
	}

	glm::vec3 getRayOnPlaneIntersectionPoint(Ray const& ray, Plane const& plane)
	{
		assert(checkRayOnPlane(ray, plane) && "Only get intersection point if you already check that they do intersect");
		return ray.getPointAtHere(getIntersectionTimeRayOnPlane(ray, plane));
	}

	bool checkPlaneOnSphere(Plane const& plane, SphereCollider const& sphere)
	{
		float nearestDistanceToPlane = distanceFromPlane(sphere.centerPos, plane);
		return fabs(nearestDistanceToPlane) < sphere.radius;
	}

	bool checkRayOnSphere(Ray const& ray, SphereCollider const& sphere)
	{
		//In case the caller doesn't have it normalized.
		glm::vec3 rayNormalized = glm::normalize(ray.direction);

		glm::vec3 endPoint = ray.startPoint + ray.length * rayNormalized;

		//Early acceptance check if start or end points are inside
		if (checkPointOnSphere(ray.startPoint, sphere) || checkPointOnSphere(endPoint, sphere))
			return true;
		
		//Vector from start point to sphere center. This is Ew --> Cw in the lecture notes
		glm::vec3 v = sphere.centerPos - ray.startPoint;

		//magnitude of the projection of v onto the ray. Using 'm' from lecture slides
		float length_m = glm::dot(v, ray.direction);

		//Early rejection test if ray is moving away
		if (length_m < 0.0f)
			return false;

		//Also reject if m is shorter than the length
		if (ray.length < length_m)
			return false;

		//Check if intersects the sphere. The lecture notes does not call square root for v at this point
		float normalSquared = glm::length2(v) - (length_m * length_m);

		//Reject if the normalSquared is more than the radius squared
		if (normalSquared > (sphere.radius * sphere.radius))
			return false;

		//There is an intersection. Another function can find the interseciton point.
		//V will have to be calculated again but it is a trivial calculation so its fine.

		return true;
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
