#pragma once
#include "AssignmentOne.h"
#include <memory>

namespace Assignment
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

	struct Plane : public CollisionObject
	{
		// The normal from the inside plane to the outside plane. The collision checks might normalize this
		glm::vec3 outwardNormal;

		//Arbitary point on the plane
		glm::vec3 pointOnPlane;

		//For rendering and interaction. Not used in collision checks
		glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

		//Render the normal as well as a ray
		Model normalModel;

		bool showPlaneNormal = true;
	};

	struct Ray : public CollisionObject
	{
		glm::vec3 startPoint;
		float length; //When t = 1 in parametric
		glm::vec3 direction;

		glm::vec3 getPointAtHere(float const t) const;
	//private:

	//	glm::vec3& getDirection() { return direction; };
	};

	//Special object that has a plane parent. Uses barycentric coordinates for the collision detection
	//
	//     0
	//   1   2
	struct Triangle
	{
		Plane triangleParentPlane;

		//Techincally the points must be aligned in the same plane
		//due to the 2D rotation rendering but the logic applies in 3D too? Not sure

		glm::vec3 pt0;
		glm::vec3 pt1;
		glm::vec3 pt2;

		//All rays will rotate counter clockwise to match the pt orientation
		Model rayA; //pt1 to pt2
		Model rayB; //pt1 to pt0
		Model rayC; //pt0 to pt2
	};

	bool checkRayOnTriangle(Triangle const& triagle, Ray const& ray);
	bool checkPointOnTriangle(Triangle const& triangle, glm::vec3 const& point);

	float distanceFromPlane(glm::vec3 const& point, Plane const& plane);

	bool checkPointOnPlane(glm::vec3 const& point, Plane const& plane, float eplsion = std::numeric_limits<float>::epsilon());

	bool collisionCheck(CollisionObject const& lhs, CollisionObject const& rhs, bool isSwap = false);

	bool checkPointOnAABB(glm::vec3 const& point, AABB const& aabb);

	bool checkPointOnSphere(glm::vec3 const& point, SphereCollider const& sphere);

	bool checkPlaneOnSphere(Plane const& plane, SphereCollider const& sphere);

	bool checkPlaneOnAABB(Plane const& plane, AABB const& aabb);

	bool checkRayOnPlane(Ray const& ray, Plane const& plane);

	bool checkRayOnSphere(Ray const& ray, SphereCollider const& sphere);

	void getIntersectionTimesRayOnSphere(Ray const& ray, Plane const& plane, float& intersectionTimeOne, 
		float& intersectionTimeTwo);

	bool checkRayOnAABB(Ray const& ray, AABB const& AABB);

	bool checkRayOnPlaneGoingOutwards(Ray const& ray, Plane const& plane);


	float getIntersectionTimeRayOnPlane(Ray const& ray, Plane const& plane);

	glm::vec3 getRayOnPlaneIntersectionPoint(Ray const& ray, Plane const& plane);

	class CollisionHelper
	{

	};
}



