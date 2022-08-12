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
		enum class nodeType
		{
			Leaf,
			Plane
		};

		class Node
		{
		public:
			nodeType type;
		};

		class LeafNode : public Node
		{
			public:

				LeafNode() {
					type = nodeType::Leaf;
				}

				std::vector<TriangleA3*> triVector; //Triangles in this node
		};


		struct splitPlane
		{
			glm::vec3 normal;
			glm::vec3 pt;

		};


		class PlaneNode : public Node
		{
			public:
				PlaneNode() {
					type = nodeType::Plane;
					leftInsideNode = nullptr;
					rightOutsideNode = nullptr;
				}

				Node* leftInsideNode;
				Node* rightOutsideNode;
		};

		class Tree
		{
		public:

			std::vector<Node*> nodeVector; //For easy deletion 

			//Maximum number of triangles per leaf
			size_t triangleMaxLeaf = 2;

			~Tree();

			PlaneNode* rootNode;
			void Init(std::vector<TriangleA3*> triAdd);
			void Clear();


			static bool isInsidePlane(TriangleA3 const& tri, glm::vec3 normal, glm::vec3 ptOnPlane);
			static glm::vec3 GetSplitPlaneNormal(glm::vec3 pt0, glm::vec3 pt1, glm::vec3 pt2);

			static bool leafContains(TriangleA3* tri, LeafNode* nodePtr);
		};
	}


}



