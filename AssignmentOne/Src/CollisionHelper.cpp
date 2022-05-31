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

	bool checkPlaneOnAABB(Plane const& plane, AABB const& aabb)
	{
		//Check if the dot products have opposite signs, and hence the plane is in the middle of the box somewhere
		float minPointDistance = distanceFromPlane(aabb.minPoint, plane);
		float maxPointDistance = distanceFromPlane(aabb.maxPoint, plane);

		if (minPointDistance > 0.0f && maxPointDistance < 0.0f)
			return true;
		if (minPointDistance < 0.0f && maxPointDistance > 0.0f)
			return true;

		return false;
	}

	bool checkPointOnTriangle(Triangle const& triangle, glm::vec3 const& point)
	{
		//Check based off edge detection
		//If the point is inside the triangle, the dot product of the point to all  edge triangle points
		//would be positive because the angle betweeen that edge and the vector to that point is acute and
		//so there is positive magnitude of the shadow and...


		// (The orientation matching the notes)
		// B      C
		//    A
		//
		glm::vec3 pt1_to_point = point - triangle.pt1;
		glm::vec3 edgeB = glm::normalize(triangle.pt0 - triangle.pt1); //Careful, ensure that the direction starts from the same pt to be checked
		float projectionLength = glm::dot(pt1_to_point, edgeB);

		if (projectionLength <= 0.0f)
			return false;

		if (projectionLength > glm::length(triangle.pt0 - triangle.pt1))
			return false;


		glm::vec3 pt0_to_point = point - triangle.pt0;
		glm::vec3 edgeA = glm::normalize(triangle.pt2 - triangle.pt0);
		
		projectionLength = glm::dot(pt0_to_point, edgeA);

		if (projectionLength <= 0.0f)
			return false;

		if (projectionLength > glm::length(triangle.pt2 - triangle.pt0))
			return false;

		glm::vec3 pt2_to_point = point - triangle.pt2;
		glm::vec3 edgeC = glm::normalize(triangle.pt0 - triangle.pt2);

		projectionLength = glm::dot(pt2_to_point, edgeC);

		if (projectionLength <= 0.0f)
			return false;

		if (projectionLength > glm::length(triangle.pt0 - triangle.pt2))
			return false;

		//Either positive or one of them's the very edge
		return true;
	}

	bool checkRayOnTriangle(Triangle const& triangle, Ray const& ray)
	{
		//get normal to any point on the plane the triangle is on
		glm::vec3 edgeB = triangle.pt1 - triangle.pt0;
		glm::vec3 edgeA = triangle.pt2 - triangle.pt0;

		glm::vec3 normal = glm::cross(edgeA, edgeB);
		Plane trianglePlane;
		trianglePlane.outwardNormal = normal;
		trianglePlane.pointOnPlane = triangle.pt0;
		if (checkRayOnPlane(ray, trianglePlane))
		{
			//Get the intersection point
			glm::vec3 pt = getRayOnPlaneIntersectionPoint(ray, trianglePlane);
			return checkPointOnTriangle(triangle, pt);
		}
		//Check if the normal is flipped

		trianglePlane.outwardNormal = -normal;
		if (checkRayOnPlane(ray, trianglePlane))
		{
			//Get the intersection point
			glm::vec3 pt = getRayOnPlaneIntersectionPoint(ray, trianglePlane);
			return checkPointOnTriangle(triangle, pt);
		}

		return false;
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

	//Note: This t value does not take in account length yet
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

	bool checkRayOnAABB(Ray const& ray, AABB const& AABB)
	{
		//First, check the ray start and end points in the box itself
		if (checkPointOnAABB(ray.startPoint, AABB))
			return true;

		glm::vec3 endPoint = ray.startPoint + ray.length * glm::normalize(ray.direction);
		if (checkPointOnAABB(endPoint, AABB))
			return true;

		//Do segment check because running out of time
		size_t numSegmentCheck = 30;
		float segmentLength = ray.length / numSegmentCheck;
		for (size_t i = 1; i < numSegmentCheck; ++i)
		{
			glm::vec3 pointToCheck = ray.startPoint + (segmentLength * i) * glm::normalize(ray.direction);
			if (checkPointOnAABB(pointToCheck, AABB))
				return true;
		}

		return false;


		//Assumption: The AABB is not rotated (...it won't be an AABB then)
		//Assumption. When facing an AABB. The min point is on the bottom left. Max point is on the top right
		//Plane forwardPlane;
		//forwardPlane.outwardNormal = worldForward;
		//forwardPlane.pointOnPlane = AABB.minPoint;

		//if (!checkRayOnPlane(ray, forwardPlane))
		//	return false;
	
		//float tz_max = getIntersectionTimeRayOnPlane(ray, forwardPlane);

		////The ray hits the AABB if the intersection of all axis tvalues is non-empty - Lecture Notes

		//Plane backPlane;
		//backPlane.outwardNormal = -worldForward;
		//backPlane.pointOnPlane = AABB.maxPoint;

		//if (!checkRayOnPlane(ray, backPlane))
		//	return false;

		//float tz_min = getIntersectionTimeRayOnPlane(ray, backPlane);

		//Plane leftPlane;
		//leftPlane.outwardNormal = -worldRight;
		//leftPlane.pointOnPlane = AABB.minPoint;


		//if (!checkRayOnPlane(ray, leftPlane))
		//	return false;

		//float tx_min = getIntersectionTimeRayOnPlane(ray, leftPlane);

		//Plane rightPlane;
		//rightPlane.outwardNormal = worldRight;
		//rightPlane.pointOnPlane = AABB.maxPoint;


		//if (!checkRayOnPlane(ray, rightPlane))
		//	return false;

		//float tx_max = getIntersectionTimeRayOnPlane(ray, rightPlane);

		//Plane upPlane;
		//upPlane.outwardNormal = worldUp;
		//upPlane.pointOnPlane = AABB.maxPoint;


		//if (!checkRayOnPlane(ray, upPlane))
		//	return false;

		//float ty_max = getIntersectionTimeRayOnPlane(ray, upPlane);

		//Plane downPlane;
		//downPlane.outwardNormal = -worldUp;
		//downPlane.pointOnPlane = AABB.minPoint;

		//if (!checkRayOnPlane(ray, downPlane))
		//	return false;

		//float ty_min = getIntersectionTimeRayOnPlane(ray, downPlane);

		//return true;

		////To Do: Get the actual intersection planes points and render them later
		////To Do: Apply proper Liang Bansly check idea


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
