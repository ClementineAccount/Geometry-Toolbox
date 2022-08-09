#pragma once
#include <glm/glm.hpp>
#include <vector> 
#include <memory>

#include "TriangleSoup.h"

namespace Assignment
{
	namespace OctTree
	{
		//Test in 2D first
		constexpr glm::vec3 upRightQuad = glm::vec3(1.0f, 1.0f, 0.0f);
		constexpr glm::vec3 upLeftQuad = glm::vec3(-1.0f, 1.0f, 0.0f);
		constexpr glm::vec3 downLeftQuad = glm::vec3(-1.0f, -1.0f, 0.0f);
		constexpr glm::vec3 downRightQuad = glm::vec3(1.0f, -1.0f, 0.0f);

		class Node
		{
		public:
			//Similar concept as AABB as we store each region as a uniform box

			glm::vec3 centerPos;
			float halfLength; //Uniform box and cube so all sides must be equal by definition (can be called radius)

			std::vector<TriangleA3*> triangleVector;

			//Node* parent; //read only ptr
			std::vector<Node> children;
		};

		class Tree
		{
		public:
			Node rootNode;

			//How many objects per cell before we perform a split
			size_t maxObjectCell;

			//Makes the root
			void Init(glm::vec3 const& centerPos, float const fullLength)
			{
				//To Do: Make this constructor or init()
				rootNode.centerPos = centerPos;
				rootNode.halfLength = fullLength * 0.5f;
			}

			//Split into the four quadrants and assign them as children
			static void SplitCell(Node& parentCell)
			{
				auto makeChild = [&](glm::vec3 cellDir)
				{
					Node child;
					child.halfLength = parentCell.halfLength * 0.5f;
					child.centerPos = parentCell.centerPos + child.halfLength * cellDir;

					return child;
				};

				parentCell.children.emplace_back(makeChild(upRightQuad));
				parentCell.children.emplace_back(makeChild(upLeftQuad));

				parentCell.children.emplace_back(makeChild(downLeftQuad));
				parentCell.children.emplace_back(makeChild(downRightQuad));
			}

			void Insert(TriangleA3* tri)
			{
				//Case 1: No children can instant insert into the root
				if (rootNode.children.empty())
				{
					rootNode.triangleVector.push_back(tri);

					//Perform our first split once we reach max objects here
					if (rootNode.triangleVector.size() == maxObjectCell)
					{
						//Divide into four quadrants
						//SplitCell(rootNode);
					}
				}
			}


			//Check which quadrant relative to this parent
			glm::vec3 whichQuad(glm::vec3 const& cellCenter, glm::vec3 const& ptA)
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
				return glm::vec3(getSign(worldRight), getSign(worldUp), 0.0f);
			}
		};
	}
}

