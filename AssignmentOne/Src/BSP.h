#pragma once
#include <glm/glm.hpp>
#include <vector> 
#include <memory>
#include "Transform.h"
#include "mesh.h"
#include "TriangleSoup.h"

namespace Assignment
{
	namespace BSP
	{
		enum nodeType
		{
			Leaf,
			Plane
		};

		class Node
		{
			nodeType type;
		};

		class LeafNode : public Node
		{
			public:
				std::vector<TriangleA3*> triVector; //Triangles in this node
		};


		class PlaneNode : public Node
		{
			public:
				Node* leftNode;
				Node* rightNode;
		};

		class Tree
		{
		public:

			Node* rootNode;
			void Init(std::vector<TriangleA3*> triAdd);

			static glm::vec3 GetSplitPlaneNormal(glm::vec3 pt0, glm::vec3 pt1, glm::vec3 pt2);
		};
	}


}



