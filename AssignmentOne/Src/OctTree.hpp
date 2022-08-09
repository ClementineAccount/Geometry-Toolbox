#pragma once
#include <glm/glm.hpp>
#include <vector> 
#include <memory>

namespace Assignment
{
	namespace OctTree
	{
		//Test in 2D first
		constexpr glm::vec3 upRightQuad = glm::vec3(1.0f, 1.0f, 0.0f);
		constexpr glm::vec3 upLeftQuad = glm::vec3(-1.0f, 1.0f, 0.0f);
		constexpr glm::vec3 downLeftQuad = glm::vec3(-1.0f, -1.0f, 0.0f);
		constexpr glm::vec3 downRightQuad = glm::vec3(1.0f, -1.0f, 0.0f);

		//Coordinates in worldPos
		template <typename Object>
		class Node
		{
		public:
			//Similar concept as AABB as we store each region as a uniform box

			glm::vec3 centerPos;
			float halfLength; //Uniform box and cube so all sides must be equal by definition (can be called radius)

			//Can pass in pointers too
			std::vector<Object> objectVector;

			//Node* parent; //read only ptr
			std::vector<Node> children;
		};

		template <typename Object>
		class Tree
		{
		public:
			using nodeType = Node<Object>;
			nodeType rootNode;

			//Makes the root
			void Init(glm::vec3 const& centerPos, float const fullLength)
			{
				//To Do: Make this constructor or init()
				rootNode.centerPos = centerPos;
				rootNode.halfLength = fullLength * 0.5f;
			}

			void Insert(Object obj)
			{
				//Case 1: No children can instant insert into the root
				if (rootNode.children.empty())
				{
					rootNode.objectVector.push_back(obj);
				}
				//push_back as you can make a tree that stores pointers rather than references
				
			}


			//Check if in the same cell using a dot product check
			glm::vec3 whichCell(glm::vec3 const& cellCenter, glm::vec3 const& ptA)
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

