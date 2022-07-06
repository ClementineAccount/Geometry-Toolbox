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
			std::unique_ptr<NodeBVH> left;
			std::unique_ptr<NodeBVH> right;

			std::unique_ptr<BV> boundingVolume;
		};

		//BVH
		template <typename BV>
		class BoundingVolumeTree
		{
		public:
			void CreateTopDown(std::vector<Object>const& objListGlobal)
			{
				for (Object const& obj : objListGlobal)
					objList.push_back(&obj);

				treeRoot = std::make_unique<NodeBVH<BV>>();
				TopDownBV(treeRoot, objList);

			}

			static void TopDownBV(std::unique_ptr<NodeBVH<BV>>& localRoot, std::vector<Object const*>& localObjectList)
			{
				std::vector<glm::vec3> objPos = GetObjectPositions((localObjectList));
				localRoot->boundingVolume = std::make_unique<BV>();
				localRoot->boundingVolume->CalculateAABB(objPos);
				localRoot->boundingVolume->UpdateBV();
			}


			std::unique_ptr<NodeBVH<BV>> treeRoot;

		private:

			//Global root

			std::vector<Object const*> objList;
		};

	}
}

