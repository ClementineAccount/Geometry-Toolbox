#include "BoundingVolume.h"
#include "Object.h"

#include <algorithm>
#include <numeric>
namespace Assignment
{
	namespace BV
	{
		void CalculateAABB(std::vector<glm::vec3>& positions, AABB& aabbRef)
		{
			auto setPos = [&](int glmOffset) {
				std::sort(positions.begin(), positions.end(), [&](glm::vec3 lhs, glm::vec3 rhs) {
					return lhs[glmOffset] < rhs[glmOffset]; });

				aabbRef.maxPoint[glmOffset] = positions[positions.size() - 1][glmOffset];
				aabbRef.minPoint[glmOffset] = positions[0][glmOffset];
			};

			////The glm offsets for glm::vec3
			int x = 0;
			int y = 1;
			int z = 2;

			setPos(x);
			setPos(y);
			setPos(z);

			//To Do: Can probably move this to a different function
			glm::vec3 fullExtent = aabbRef.maxPoint - aabbRef.minPoint;
			aabbRef.halfExtent = fullExtent * 0.5f;
			aabbRef.centerPos = aabbRef.minPoint + aabbRef.halfExtent;

			auto setScale = [&](int glmOffset) {
				aabbRef.scale[glmOffset] = fullExtent[glmOffset];
			};

			setScale(x);
			setScale(y);
			setScale(z);

			aabbRef.Update();
		}

		//Update it based off the current position
		void AABB::Update()
		{
			model.pos = centerPos;
			model.scale = scale;
		}

		glm::vec3 SplitPointMean(std::vector<Object*> const& objList)
		{
			std::vector<glm::vec3> centerList;
			for (auto const& obj : objList)
			{
				glm::vec3 center = obj->bv.getCenter();
				centerList.push_back(center);
			}

			glm::vec3 centerPoint = glm::vec3(0.0f, 0.0f, 0.0f);
			for (glm::vec3 const& point : centerList)
				centerPoint += point;
			centerPoint /= centerList.size();

			return centerPoint;
		}

		glm::vec3 SplitPlaneAxis(glm::vec3 splitAxisNormal)
		{
			return splitAxisNormal;
		}

		void SplitObjectRegions(objectSplitList objList, glm::vec3 splitPlanePoint, glm::vec3 splitPlaneNormal)
		{
			auto positiveHalfSpace = [&](glm::vec3 point) {
				glm::vec3 dir = point - splitPlanePoint;
				return glm::dot(dir, splitPlaneNormal) > 0.0f;
			};

			for (auto const& obj : objList.allObjects)
			{
				if (positiveHalfSpace(obj->bv.getCenter()))
				{
					objList.positiveObjects.push_back(obj);
				}
				else
				{
					objList.negativeObjects.push_back(obj);
				}
			}
		}

		void SplitObjectRegions(std::vector<Object*>& objList, glm::vec3 splitPlanePoint, glm::vec3 splitPlaneDir)
		{
			objectSplitList obsl;
			obsl.allObjects = objList;
			SplitObjectRegions(obsl, splitPlanePoint, splitPlaneDir);
		}
	}
}
