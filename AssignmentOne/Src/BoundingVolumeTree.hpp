#pragma once
#include "BoundingVolume.h"
#include "Object.h"

#include <iostream>

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

		//List of nodes per height
		template <typename BV>
		struct nodeList
		{
			std::vector<std::shared_ptr<NodeBVH<BV>>> nodes;
		};

		//To speed up the amount of time it takes to do each layer, we should cache important infomation
		//Use this infomation for the axis separation after we have it
		//struct ObjectMeta
		//{
		//	glm::vec3 maxPoint;
		//	glm::vec3 minPoint;
		//};

		//BVH
		template <typename BV>
		class BoundingVolumeTree
		{
		public:

			void Clear()
			{
				heightMap.clear();
				objList.clear();
			}

			void CreateTopDown(std::vector<Object>const& objListGlobal)
			{
				Clear();

				for (Object const& obj : objListGlobal)
					objList.push_back(&obj);

				treeRoot = std::make_shared<NodeBVH<BV>>();
				size_t currHeight = 0;
				AddToHeight(currHeight, treeRoot);
				//heightMap.insert({ currHeight, treeRoot });

				std::cout << "Constructing BVH Tree (Top Down)" << std::endl;
				TopDownBV(treeRoot, objList, currHeight);

				std::cout << "Completed BVH Tree (Top Down)" << std::endl;
				std::cout << "BVH Tree Max Height: " << totalHeight - 1 << std::endl;
			}

			void TopDownBV(std::shared_ptr<NodeBVH<BV>>& localRoot, std::vector<Object const*>& localObjectList, size_t currHeight)
			{
				std::cout << "TopDownBVH Height: " << currHeight << std::endl;

				if (localObjectList.size() <= 1)
				{
					if (currHeight >= totalHeight)
						totalHeight = currHeight;
					return;
				}
				if (currHeight + 1 >= maxHeight)
					return;

				if (currHeight >= totalHeight)
					totalHeight = currHeight;




				std::vector<glm::vec3> objPos = GetObjectPositions((localObjectList));
				localRoot->boundingVolume = std::make_shared<BV>();
				localRoot->boundingVolume->CalculateBV(objPos);
				localRoot->boundingVolume->UpdateBV();

				std::cout << "Number Object Positions: " << objPos.size() << std::endl;

				glm::vec3 spreadAxis = axisOffsets::axisToDirection(largestSpreadAxis(objPos));
				glm::vec3 centerMean = calculatePositionMean(objPos);
					
				//The split plane for this example uses the centerMean as a point on the plane and the spreadAxis as the normal
				glm::vec3 splitPoint = centerMean;
				glm::vec3 planeNormal = spreadAxis;

				std::vector<Object const*> leftList;
				std::vector<Object const*> rightList;

				for (Object const* obj : localObjectList)
				{
					glm::vec3 bvCenter = obj->bvPrimitiveAABB.getCenter();
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

				AddToHeight(currHeight, localRoot->left);
				TopDownBV(localRoot->left, leftList, currHeight);

				localRoot->right = std::make_shared<NodeBVH<BV>>();
				AddToHeight(currHeight, localRoot->right);
				TopDownBV(localRoot->right, rightList, currHeight);
			}

			void AddToHeight(size_t height, std::shared_ptr<NodeBVH<BV>>& node)
			{
				if (heightMap.count(height) == 0)
				{
					nodeList<BV> nl;
					heightMap.insert({ height, nl });
				}
				heightMap.at(height).nodes.push_back(node);
			}

			std::shared_ptr<NodeBVH<BV>> treeRoot;
			std::unordered_map<size_t, nodeList<BV>> heightMap;

			int GetHeight()
			{
				//Last height doesn't have BVHs
				return totalHeight - 1;
			}

			void SetColorHeight(int height, glm::vec3 const& color)
			{
				if (heightMap.count(height) == 0)
					return;

				for (auto& bv : heightMap.at(height).nodes)
				{
					if (bv->boundingVolume != nullptr)
					{
						bv->boundingVolume->model.color = color;
					}
				}
			}

		private:

			int totalHeight = 0;
			int maxHeight = 8;

			//Map that stores pointers to nodes that are of a certain height

			std::vector<Object const*> objList;
			//std::unordered_map<Object const*, std::vector<glm::vec3>> objListPosCache; 
		};

	}
}

