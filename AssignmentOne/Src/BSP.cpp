#pragma once
#include "BSP.h"

namespace Assignment
{
	namespace BSP
	{

		glm::vec3 Tree::GetSplitPlaneNormal(glm::vec3 pt0, glm::vec3 pt1, glm::vec3 pt2)
		{
			glm::vec3 vectorA = pt1 - pt0;
			glm::vec3 vectorB = pt2 - pt0;

			glm::vec3 vectorC = glm::cross(vectorA, vectorB);

			glm::vec3 normal = glm::cross(vectorA, -vectorC);
			return normal;
		}

		bool Tree::isInsidePlane(TriangleA3 const& tri, glm::vec3 normal, glm::vec3 ptOnPlane)
		{


			//Since they do share points we just disqualify if one is out for now. Later on we have to add thershold or something
			glm::vec3 checkVector = tri.ptA - ptOnPlane;

			if (glm::dot(checkVector, normal) < 0.0f)
				return false;

			if (glm::dot(ptOnPlane - tri.ptB, normal) < 0.0f)
				return false;

			return true;
		}

		Tree::~Tree()
		{
			Clear();
		}

		void Tree::Clear()
		{
			for (Node* node : nodeVector)
				delete node;
		}

		void Tree::Init(std::vector<TriangleA3*> triAdd)
		{
			//To Do: Error code if empty?
			if (triAdd.empty())
				return;

			//To Do: Make this a stack/recursion 

			//Create the initial root plane from the first edge of the first triangle
			glm::vec3 halfPlaneNormal = GetSplitPlaneNormal(triAdd[0]->ptA, triAdd[0]->ptB, triAdd[0]->ptC);
			
			glm::vec3 planePt = triAdd[0]->ptA;

			rootNode = new PlaneNode();
			nodeVector.push_back(rootNode);

			//Sort the two halves
			std::vector<TriangleA3*> insideList; //Left
			std::vector<TriangleA3*> outsideList; //Right

			//To Do: Refactor?
			for (auto const& tri : triAdd)
			{
				if (isInsidePlane(*tri, halfPlaneNormal, planePt))
					insideList.push_back(tri);
				else
					outsideList.push_back(tri);
			}

			//To Do: The herustic check for left/right leaf node
			rootNode->leftInsideNode = new LeafNode();
			rootNode->rightOutsideNode = new LeafNode();

			nodeVector.push_back(rootNode->leftInsideNode);
			nodeVector.push_back(rootNode->rightOutsideNode);

			for (auto const& tri : insideList)
				static_cast<LeafNode*> (rootNode->leftInsideNode)->triVector.push_back(tri);
			for (auto const& tri : outsideList)
				static_cast<LeafNode*> (rootNode->rightOutsideNode)->triVector.push_back(tri);
		}

		bool Tree::leafContains(TriangleA3* tri, LeafNode* nodePtr)
		{
			for (TriangleA3* triCheck : nodePtr->triVector)
			{
				//Originally wanted to compare memory address but that causes issues 
				if (tri->ptA == triCheck->ptA && tri->ptB == triCheck->ptB && tri->ptC == triCheck->ptC)
					return true;
			}

			return false;
		}
	}

}