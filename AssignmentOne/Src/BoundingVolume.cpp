#include "BoundingVolume.h"
#include "Object.h"

#include <algorithm>
#include <numeric>
namespace Assignment
{
	namespace BV
	{
		void BoundingVolume::AddObject(Object const* obj)
		{
			objectList.push_back(obj);
		}

		void AABB::CalculateAABB(std::vector<glm::vec3>& positions)
		{

			auto setPos = [&](int glmOffset) {
				std::sort(positions.begin(), positions.end(), [&](glm::vec3 lhs, glm::vec3 rhs) {
					return lhs[glmOffset] < rhs[glmOffset]; });

				maxPoint[glmOffset] = positions[positions.size() - 1][glmOffset];
				minPoint[glmOffset] = positions[0][glmOffset];
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

			auto setScale = [&](int glmOffset) {
				scale[glmOffset] = fullExtent[glmOffset];
			};

			setScale(x);
			setScale(y);
			setScale(z);
		}

		void AABB::UpdateBV()
		{
			std::vector<glm::vec3> positions;
			std::for_each(objectList.begin(), objectList.end(), [&](Object const* obj) {
				std::vector<glm::vec3> objPos = GetObjectPositions(*obj);
				positions.insert(positions.end(), objPos.begin(), objPos.end());
				});

			CalculateAABB(positions);
			Update();
		}

		//Update it based off the current position
		void AABB::Update()
		{
			//Also runs the calculation algo

			model.pos = centerPos;
			model.scale = scale;
		}





		////This could be a list of BVs instead?
		//glm::vec3 SplitPointMean(std::vector<Object*> const& objList)
		//{
		//	std::vector<glm::vec3> centerList;
		//	for (auto const& obj : objList)
		//	{
		//		centerList.push_back(GetCenterObject(*obj));
		//	}

		//	glm::vec3 centerPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		//	for (glm::vec3 const& point : centerList)
		//		centerPoint += point;
		//	centerPoint /= centerList.size();

		//	return centerPoint;
		//}

		//glm::vec3 SplitPlaneAxis(glm::vec3 splitAxisNormal)
		//{
		//	return splitAxisNormal;
		//}

		//void SplitObjectRegions(objectSplitList objList, glm::vec3 splitPlanePoint, glm::vec3 splitPlaneNormal)
		//{
		//	auto positiveHalfSpace = [&](glm::vec3 point) {
		//		glm::vec3 dir = point - splitPlanePoint;
		//		return glm::dot(dir, splitPlaneNormal) > 0.0f;
		//	};

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
