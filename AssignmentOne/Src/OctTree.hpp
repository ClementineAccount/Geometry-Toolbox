#pragma once
#include <glm/glm.hpp>
#include <vector> 
#include <memory>

namespace Assignment
{
	namespace OctTree
	{
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
		};

		template <typename Object>
		class Tree
		{
		public:
			std::unique_ptr<Node<Object>> rootNode;

			//Makes the root
			void Init(glm::vec3 const& centerPos, float const fullLength)
			{
				rootNode = std::make_unique<Node<Object>>();

				//To Do: Make this constructor or init()
				rootNode->centerPos = centerPos;
				rootNode->halfLength = fullLength * 0.5f;
			}

			void Insert(Object obj)
			{

				//push_back as you can make a tree that stores pointers rather than references
				rootNode->objectVector.push_back(obj);
			}
		};
	}
}

