#include "BoundingVolume.h"
#include "Object.h"
#include <memory>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace Assignment
{
	namespace BV
	{
		glm::vec3 axisOffsets::axisToDirection(int component)
		{

			if (component == axisOffsets::x)
				return worldRight;

			if (component == axisOffsets::y)
				return worldUp;

			if (component == axisOffsets::z)
				return worldForward;
		}

		void AABB::CalculateAABB(Object const& obj)
		{
			std::vector<glm::vec3> pos = GetObjectPositions(obj);
			CalculateAABB(pos);
		}

		void AABB::CalculateAABB(std::vector<glm::vec3>& positions)
		{
			auto setPos = [&](int component) {
				std::sort(positions.begin(), positions.end(), [&](glm::vec3 const& lhs, glm::vec3 const& rhs) {
					return lhs[component] < rhs[component]; });

				maxPoint[component] = positions[positions.size() - 1][component];
				minPoint[component] = positions[0][component];
			};

			////The glm offsets for glm::vec3
			int x = 0;
			int y = 1;
			int z = 2;

			setPos(x);
			setPos(y);
			setPos(z);

			//To Do: Can probably move this to a different function
			glm::vec3 fullExtent = maxPoint - minPoint;
			halfExtent = fullExtent * 0.5f;
			centerPos = minPoint + halfExtent;

			auto setScale = [&](int component) {
				scale[component] = fullExtent[component];
			};

			setScale(x);
			setScale(y);
			setScale(z);
		}

		void AABB::CalculateBV(Object const& obj)
		{
			CalculateAABB(obj);
		}

		void AABB::CalculateBV(std::vector<glm::vec3>& positions)
		{
			CalculateAABB(positions);
		}

		//Update it based off the current position
		void AABB::UpdateBV()
		{
			meshID = MeshNames::cube;
			model.pos = centerPos;
			model.scale = scale;
		}

		void Sphere::UpdateBV()
		{
			meshID = MeshNames::sphere;
			model.pos = centerPos;
			model.scale = glm::vec3(radius, radius, radius);
		}


		void Sphere::CalculateBV(Object const& obj)
		{
			std::vector<glm::vec3> pos = GetObjectPositions(obj);
			CalculateRitters(pos);
		}

		void Sphere::CalculateBV(std::vector<glm::vec3>& positions)
		{
			CalculateRitters(positions);
		}


		void Sphere::CalculateRitters(std::vector<glm::vec3>& positions)
		{
			std::cout << "Caulcating Ritters\n";
			glm::vec3 startPoint = positions[0];
			
			//Get furthest point from here
			using ptDistancePair = std::pair<float, glm::vec3>;

			auto findFarPoint = [&](glm::vec3 const& referencePoint, std::vector<glm::vec3> const& posList)
			{
				ptDistancePair furthestPoint({ 0.0f, referencePoint });
				for (auto const& pos : posList)
				{
					glm::vec3 dirVector = referencePoint - pos;
					if (glm::dot(dirVector, dirVector) > furthestPoint.first)
					{
						//Note: this is the distance squared so square it after you are done with everything
						furthestPoint.first = glm::dot(dirVector, dirVector);
						furthestPoint.second = pos;
					}
				}
				return furthestPoint.second;
			};

			glm::vec3 secondPt = findFarPoint(startPoint, positions);
			glm::vec3 thirdPt = findFarPoint(secondPt, positions);
			glm::vec3 dir = (thirdPt - secondPt) * 0.5f;


			centerPos = secondPt + dir;
			radius = glm::length(dir);
			auto allPointsOutside = [&]()
			{
				std::vector<glm::vec3> outsidePoints;
				for (auto const& pos : positions)
				{
					if (glm::distance(pos, centerPos) > radius)
						outsidePoints.push_back(pos);
				}
				return outsidePoints;
			};

			std::vector<glm::vec3> outsidePoints = allPointsOutside();
			//outsidePoints.clear();

			//Something is wrong with my second pass. Will have to investigate why
			while (!outsidePoints.empty())
			{
				glm::vec3 farOutsidePoint = findFarPoint(centerPos, outsidePoints);
				glm::vec3 dir = glm::normalize(farOutsidePoint - centerPos);
				glm::vec3 furthestPointEdge = centerPos - (dir * radius);

				glm::vec3 diameterVector = (farOutsidePoint - furthestPointEdge);
				radius = glm::length(diameterVector) * 0.51f; //have to add a slight buffer to prevent infinite loop sometimes
				centerPos = furthestPointEdge + diameterVector * 0.5f;

				outsidePoints.clear();
				outsidePoints = allPointsOutside();
			}
		}


		glm::vec3 calculatePositionMean(std::vector<glm::vec3> const& positions)
		{
			glm::vec3 centerMean = glm::vec3(0.0f, 0.0f, 0.0f);

			for (glm::vec3 const& p : positions)
				centerMean += p;
			centerMean /= positions.size();

			return centerMean;
		}

		glm::vec3 calculateCenterMean(BoundingVolume const* bv, size_t numBV)
		{
			std::vector<glm::vec3> centerPosList;
			for (size_t i = 0; i < numBV; ++i)
				centerPosList.push_back(bv[i].getCenter());
			return calculatePositionMean(centerPosList);
		}

		int largestSpreadAxis(std::vector<glm::vec3> const& positions)
		{
			std::vector<glm::vec3> positionsTemp = positions;
			
			auto setPos = [&](int component, float& distance) {
				std::sort(positionsTemp.begin(), positionsTemp.end(), [&](glm::vec3 const& lhs, glm::vec3 const& rhs) {
					return lhs[component] < rhs[component]; });

				distance = positionsTemp[positions.size() - 1][component] - positionsTemp[0][component];
			};

			////The glm offsets for glm::vec3
			int x = 0;
			int y = 1;
			int z = 2;

			struct distanceIdentity
			{
				float distance = 0.0f;
				int distanceID = 0;
			};

			std::vector<distanceIdentity> distanceSet;
			distanceSet.resize(3);

			distanceSet[z].distanceID = z;
			distanceSet[y].distanceID = y;
			distanceSet[x].distanceID = x;

			setPos(x, distanceSet[x].distance);
			setPos(y, distanceSet[y].distance);
			setPos(z, distanceSet[z].distance);

			std::sort(distanceSet.begin(), distanceSet.end(), [&](distanceIdentity lhs, distanceIdentity rhs) 
				{ return lhs.distance < rhs.distance;});

			return distanceSet.back().distanceID;
		}


		//void BoundingVolumeTree::SplitBV(splitParameters bvToSplit, splitParameters bvLeft, splitParameters bvRight)
		//{
		//	glm::vec3 centerMean = calculateCenterMean(bvToSplit);


		//	std::vector<Object const*> totalObjectList;
		//	//for (size_t i = 0; i < bvToSplit.numBV; ++i)
		//	//{
		//	//	totalObjectList.insert(totalObjectList.begin(), bvToSplit.bv[i].objectList.begin(), bvToSplit.bv[i].objectList.end());
		//	//}

		//	glm::vec3 spreadAxis = axisOffsets::axisToDirection(largestSpreadAxis(GetObjectPositions(totalObjectList)));

		//	//The split plane for this example uses the centerMean as a point on the plane and the spreadAxis as the normal
		//	glm::vec3 splitPoint = centerMean;
		//	glm::vec3 planeNormal = spreadAxis;

		//	for (Object const* obj : totalObjectList)
		//	{


		//	}
		//}


		//////This could be a list of BVs instead?
		////glm::vec3 SplitPointMean(std::vector<Object*> const& objList)
		////{
		////	std::vector<glm::vec3> centerList;
		////	for (auto const& obj : objList)
		////	{
		////		centerList.push_back(GetCenterObject(*obj));
		////	}

		////	glm::vec3 centerPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		////	for (glm::vec3 const& point : centerList)
		////		centerPoint += point;
		////	centerPoint /= centerList.size();

		////	return centerPoint;
		////}

		////glm::vec3 SplitPlaneAxis(glm::vec3 splitAxisNormal)
		////{
		////	return splitAxisNormal;
		////}

		////void SplitObjectRegions(objectSplitList objList, glm::vec3 splitPlanePoint, glm::vec3 splitPlaneNormal)
		////{
		////	auto positiveHalfSpace = [&](glm::vec3 point) {
		////		glm::vec3 dir = point - splitPlanePoint;
		////		return glm::dot(dir, splitPlaneNormal) > 0.0f;
		////	};

		//	for (auto const& obj : objList.allObjects)
		//	{
		//		if (positiveHalfSpace(obj->bv.getCenter()))
		//		{
		//			objList.positiveObjects.push_back(obj);
		//		}
		//		else
		//		{
		//			objList.negativeObjects.push_back(obj);
		//		}
		//	}
		//}

		//void SplitObjectRegions(std::vector<Object*>& objList, glm::vec3 splitPlanePoint, glm::vec3 splitPlaneDir)
		//{
		//	objectSplitList obsl;
		//	obsl.allObjects = objList;
		//	SplitObjectRegions(obsl, splitPlanePoint, splitPlaneDir);
		//}
	}
}
