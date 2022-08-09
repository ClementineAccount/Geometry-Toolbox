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

			glm::vec3 centerPos;
			float halfLength; //Uniform box and cube so all sides must be equal by definition (can be called radius)

			std::vector<TriangleA3*> triangleVector;

			//Node* parent; //read only ptr

			std::unordered_map<glm::vec3, Node> childMap;
			std::vector<Node> children;

			//For rendering
			bool isRendering = true;

			Transform transform;
			//Mesh* boxMesh; //Should be an aabb so cube
		};

		class Tree
		{
		public:
			Node rootNode;

			//For easy rendering. Allows O(n) rendering at cost of O(n) space complexity
			std::vector<Node*> allNodes; 


			//How many objects per cell before we perform a split
			size_t maxObjectCell = 5;

			//Makes the root
			void Init(glm::vec3 const& centerPos, float const fullLength)
			{
				//To Do: Make this constructor or init()
				rootNode.centerPos = centerPos;

				rootNode.Init(centerPos, fullLength);
				allNodes.emplace_back(&rootNode);
			}

			//Split into the four quadrants and assign them as children
			void SplitCell(Node& parentCell)
			{
				auto makeChild = [&](glm::vec3 cellDir)
				{
					Node child;
					glm::vec3 center = parentCell.centerPos + parentCell.halfLength * 0.5f * cellDir;
					child.Init(center, parentCell.halfLength);
					child.transform.color = glm::vec3(0.0f, 0.0f, 1.0f);

					parentCell.childMap.emplace(cellDir, child );
					allNodes.push_back(&parentCell.childMap.at(cellDir));
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
				if (!currNode->children.empty())
				{
					glm::vec3 offsetDir = whichOct(currNode->centerPos, tri->ptA);
					currNode->childMap.at(offsetDir).triangleVector.push_back(tri);
				}

				//Case 1: No children can instant insert into the root
				if (currNode->children.empty())
				{
					currNode->triangleVector.push_back(tri);

					//Perform split once we reach max objects here
					if (currNode->triangleVector.size() == maxObjectCell)
					{
						//Divide into the eight
						SplitCell(rootNode);


						std::vector<TriangleA3*> triVectorCopy;
						//triVectorCopy.assign(rootNode.triangleVector.begin(), rootNode.triangleVector.end());
						rootNode.triangleVector.clear();


						for (auto& tri : triVectorCopy)
							Insert(tri);
					}
				}
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
				return glm::vec3(getSign(worldRight), getSign(worldUp), getSign(worldRight));
			}
		};
	}
}

