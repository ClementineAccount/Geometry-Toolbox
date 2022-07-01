#include "BoundingVolume.h"
#include "Object.h"

#include <algorithm>

namespace Assignment
{
	namespace BV
	{
		std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList, size_t start, size_t end)
		{
			//maybe can just store pointer to the positions? that way there will be no need allocate extra memory
			std::vector<glm::vec3> positions;

			//The caller will have to pass in (size - 1) if they wish to go until the end
			for (size_t i = start; i <= end; ++i)
			{
				glm::mat4 modelMat = calculateModel(objectList[i].transform);
				for (auto const& localPos : objectList[i].objectMesh.meshVertices.positions)
				{
					glm::vec4 modelPos = modelMat * glm::vec4(localPos.x, localPos.y, localPos.z, 1.0f);
					positions.push_back(glm::vec3(modelPos.x, modelPos.y, modelPos.z));
				}

			}

			return positions;
		}

		std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList)
		{
			return GetObjectPositions(objectList, 0, objectList.size() - 1);
		}

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


	}
}
