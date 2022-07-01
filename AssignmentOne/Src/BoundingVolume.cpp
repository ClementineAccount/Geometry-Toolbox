#include "BoundingVolume.h"
#include "Object.h"

#include <algorithm>

namespace Assignment
{
	namespace BV
	{
		void CalculateAABB(std::vector<Object> const& objects, AABB& aabbRef)
		{
			////maybe can just store pointer to the positions? that way there will be no need allocate extra memory
			//std::vector<glm::vec3> positions;


			//for (Object const& obj : objects)
			//{
			//	positions.emplace_back(obj.objectMesh.meshVertices.positions);
			//}

			////Get the maximum x, y and z points in the list of objects

			//auto setPos = [&](int glmOffset) {
			//	std::sort(positions.begin(), positions.end(), [&](glm::vec3 lhs, glm::vec3 rhs) {
			//		lhs[glmOffset] < rhs[glmOffset]; });

			//	aabbRef.maxPoint[glmOffset] = positions[positions.size()][glmOffset];
			//	aabbRef.minPoint[glmOffset] = positions[0][glmOffset];
			//};

			////The glm offsets for glm::vec3
			//int x = 0;
			//int y = 1;
			//int z = 2;

			//setPos(x);
			//setPos(y);
			//setPos(z);
		}
	}
}
