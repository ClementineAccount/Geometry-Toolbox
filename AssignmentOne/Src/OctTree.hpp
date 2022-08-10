#pragma once
#include <glm/glm.hpp>
#include <vector> 
#include <memory>
#include "Transform.h"
#include "mesh.h"
#include "TriangleSoup.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


namespace Assignment
{
	namespace OctTree
	{
		constexpr glm::vec3 upRightQuad = glm::vec3(1.0f, 1.0f, 1.0f);
		constexpr glm::vec3 upLeftQuad = glm::vec3(-1.0f, 1.0f, 1.0f);

		constexpr glm::vec3 upRightQuadMirror = glm::vec3(1.0f, 1.0f, -1.0f);
		constexpr glm::vec3 upLeftQuadMirror = glm::vec3(-1.0f, 1.0f, -1.0f);

		constexpr glm::vec3 downLeftQuad = glm::vec3(-1.0f, -1.0f, -1.0f);
		constexpr glm::vec3 downRightQuad = glm::vec3(1.0f, -1.0f, -1.0f);

		constexpr glm::vec3 downLeftQuadMirror = glm::vec3(-1.0f, -1.0f, 1.0f);
		constexpr glm::vec3 downRightQuadMirror = glm::vec3(1.0f, -1.0f, 1.0f);



		class Node
		{
		public:
			//Similar concept as AABB as we store each region as a uniform box
			void Init(glm::vec3 const& _centerPos, float fullLength)
			{
				centerPos = _centerPos;
				halfLength = fullLength * 0.5f;
				transform.pos = centerPos; //pivot center
				transform.scale = glm::vec3(fullLength, fullLength, fullLength);
				transform.color = glm::vec3(1.0f, 0.0f, 0.0f);
			}

			void Clear()
			{
				//Does not remove memory
				triangleVector.clear();
			}

			glm::vec3 centerPos;
			float halfLength; //Uniform box and cube so all sides must be equal by definition (can be called radius)

			std::vector<TriangleA3*> triangleVector;

			//Node* parent; //read only ptr

			std::unordered_map<glm::vec3, Node*> childMap;
			//std::vector<Node> children;

			//For rendering
			bool isRendering = true;

			size_t level;

			Transform transform;
			//Mesh* boxMesh; //Should be an aabb so cube
		};

		class Tree
		{
		public:

			~Tree() 
			{

			}

			Node rootNode;

			size_t currLevel = 0;

			//For easy rendering. Allows O(n) rendering at cost of O(n) space complexity
			//std::vector<Node*> allNodes; 

			//For displaying the nodes
			std::unordered_map <size_t, std::vector<Node*>> nodesMap;
			std::unordered_map<size_t, glm::vec3> colorLevelMap;


			Node* GetNodeWithObj(TriangleA3* tri)
			{
				for (size_t i = 0; i <= currLevel; ++i)
				{
					for (Node* node : nodesMap[i])
					{
						for (auto const& triCheck : node->triangleVector)
						{
							if (triCheck == tri)
								return node;
						}
					}
				}
				return nullptr;
			}


			//How many objects per cell before we perform a split
			size_t maxObjectCell = 2;

			//Makes the root
			void Init(glm::vec3 const& centerPos, float const fullLength, size_t _maxObjectCell = 2)
			{
				currLevel = 0;

				//To Do: Make this constructor or init()
				rootNode.centerPos = centerPos;
				rootNode.level = 0;
				currLevel = 0;
				maxObjectCell = _maxObjectCell;

				rootNode.Init(centerPos, fullLength);
				nodesMap.emplace(0, std::vector<Node*>());
				nodesMap.at(0).push_back(&rootNode);

				colorLevelMap.emplace(0, glm::vec3(1.0f, 0.0f, 0.0f));
				colorLevelMap.emplace(1, glm::vec3(0.0f, 1.0f, 0.0f));
				colorLevelMap.emplace(2, glm::vec3(0.0f, 0.0f, 1.0f));
				colorLevelMap.emplace(3, glm::vec3(0.0f, 1.0f, 1.0f));

			}

			//Split into the four quadrants and assign them as children
			void SplitCell(Node& parentCell)
			{
				size_t level = parentCell.level + 1;
				currLevel = level;
				nodesMap.emplace(level, std::vector<Node*>());

				auto makeChild = [&](glm::vec3 cellDir)
				{
					Node* child = new Node();
					child->level = level;
					glm::vec3 center = parentCell.centerPos + parentCell.halfLength * 0.5f * cellDir;
					child->Init(center, parentCell.halfLength);
					child->transform.color = colorLevelMap[child->level];

					parentCell.childMap.emplace(cellDir, child );
					nodesMap.at(level).push_back(parentCell.childMap.at(cellDir));
				};


				makeChild(upRightQuad);
				makeChild(upLeftQuad);
				makeChild(upRightQuadMirror);
				makeChild(upLeftQuadMirror);
				makeChild(downLeftQuad);
				makeChild(downRightQuad);
				makeChild(downLeftQuadMirror);
				makeChild(downRightQuadMirror);
			}

			void Insert(TriangleA3* tri)
			{
				Node* currNode = &rootNode;
				while (!currNode->childMap.empty())
				{
					glm::vec3 offsetDir = whichOct(currNode->centerPos, tri->ptA);
					currNode = currNode->childMap.at(offsetDir);
				}

				if (currNode->childMap.empty())
				{
					tri->color = colorLevelMap[currNode->level];

					//Perform split once we reach max objects here (count from 0)
					if (currNode->triangleVector.size() + 1 > maxObjectCell)
					{
						//Divide into the eight
						SplitCell(*currNode);
						for (auto& triAddAgain : currNode->triangleVector)
							Insert(triAddAgain);

						currNode->triangleVector.clear();
						Insert(tri);
					}
					else
					{
						currNode->triangleVector.push_back(tri);
					}
				}
			}


			//For resetting the tree. No dynamic memory allocation
			void Clear()
			{


				currLevel = 0;

				for (auto& it : nodesMap)
				{
					for (size_t i = 0; i < it.second.size(); ++i)
					{
						Node* ptr = it.second[i];
						if (!ptr->childMap.empty())
							ptr->childMap.clear();

						if (ptr != &rootNode && ptr != nullptr)
							delete ptr;

					}

					//for (auto& it2 : it.second)
					//{
					//	if (!it2->childMap.empty())
					//		it2->childMap.clear();

					//	/*if (it2 != nullptr)
					//		delete it2;*/
					//}
						
				}

				nodesMap.clear();
				colorLevelMap.clear();


			}


			//Check which quadrant relative to this parent
			glm::vec3 whichOct(glm::vec3 const& cellCenter, glm::vec3 const& ptA)
			{
				/*
				The idea (made myself):
				The objects are not in the same cell if they are not in the same quadrant
				and we can check their quadrants via dot product operations

				 Example 2D:
						+------------+
						|  C   | A   |
						|      |   B |
						|------+-----|
						|      |     |
						|      |     |
						|------------|

				 If we take the vector from A to the center and dot it with
				 the vertical and horizontal axis, we can verify that A and B
				 will both have a positive result with the horizontal and vertical, while
				 C has a negative result with the horizontal.

				 We can conclude which cell a point belongs to based off the sign
				*/

				glm::vec3 centerToA = ptA - cellCenter;

				auto getSign = [&](glm::vec3 axisCheck) {
					return glm::dot(centerToA, axisCheck) >= 0.0f ? 1.0f : -1.0f;
				};
				return glm::vec3(getSign(worldRight), getSign(worldUp), getSign(worldForward));
			}
		};
	}
}

