#pragma once
#include "BoundingVolume.h"
#include "Object.h"

namespace Assignment
{
	namespace BV
	{
		//For BVH trees
		template <typename BV>
		struct NodeBVH
		{
			std::shared_ptr<NodeBVH> left;
			std::shared_ptr<NodeBVH> right;

			std::shared_ptr<BV> boundingVolume;
		};

		//BVH
		template <typename BV>
		class BoundingVolumeTree
		{
		public:

			void CreateTopDown(std::vector<Object>const& objListGlobal)
			{

				objList.clear();

				for (Object const& obj : objListGlobal)
					objList.push_back(&obj);

				treeRoot = std::make_shared<NodeBVH<BV>>();
				size_t currHeight = 0;
				heightMap.insert({ currHeight, treeRoot });
				TopDownBV(treeRoot, objList, currHeight);
			}


			void TopDownBV(std::shared_ptr<NodeBVH<BV>>& localRoot, std::vector<Object const*>& localObjectList, size_t currHeight)
			{
				//To Do: Add leaf node support
				if (localObjectList.size() <= 1)
					return;

				std::vector<glm::vec3> objPos = GetObjectPositions((localObjectList));
				localRoot->boundingVolume = std::make_shared<BV>();
				localRoot->boundingVolume->CalculateAABB(objPos);
				localRoot->boundingVolume->UpdateBV();

				glm::vec3 spreadAxis = axisOffsets::axisToDirection(largestSpreadAxis(GetObjectPositions(localObjectList)));
				
				glm::vec3 centerMean = calculatePositionMean(objPos);
				
				//The split plane for this example uses the centerMean as a point on the plane and the spreadAxis as the normal
				glm::vec3 splitPoint = centerMean;
				glm::vec3 planeNormal = spreadAxis;

				std::vector<Object const*> leftList;
				std::vector<Object const*> rightList;

				for (Object const* obj : localObjectList)
				{
					glm::vec3 bvCenter = obj->bvPrimitive.getCenter();
					glm::vec3 dir = splitPoint - bvCenter;
					if (glm::dot(dir, planeNormal) > 0.0f)
					{
						leftList.push_back(obj);
					}
					else
					{
						rightList.push_back(obj);
					}
				}

				++currHeight;
				localRoot->left = std::make_shared<NodeBVH<BV>>();
				heightMap.insert({ currHeight, localRoot->left });
				TopDownBV(localRoot->left, leftList, currHeight);

				localRoot->right = std::make_shared<NodeBVH<BV>>();
				heightMap.insert({ currHeight, localRoot->right });
				TopDownBV(localRoot->right, rightList, currHeight);
			}

			std::shared_ptr<NodeBVH<BV>> treeRoot;

		private:

			size_t totalHeight = 0;

			//Map that stores pointers to nodes that are of a certain height
			std::unordered_map<size_t, std::shared_ptr <NodeBVH<BV>>> heightMap;
			std::vector<Object const*> objList;
		};

	}
}

