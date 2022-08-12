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
			rootNode = new PlaneNode();
			nodeVector.push_back(rootNode);

			//BFS-like insertion
			std::queue<TriangleA3*> triAddQueue;
			std::queue<splitPlane> splitPaneQueue;

			for (TriangleA3* tri : triAdd)
			{
				triAddQueue.push(tri);
				splitPlane currPlane;

				currPlane.normal = GetSplitPlaneNormal(triAddQueue.front()->ptA, triAddQueue.front()->ptB, triAddQueue.front()->ptC);
				currPlane.pt = triAddQueue.front()->ptA;
				splitPaneQueue.push(currPlane);

				currPlane.normal = GetSplitPlaneNormal(triAddQueue.front()->ptB, triAddQueue.front()->ptC, triAddQueue.front()->ptA);
				currPlane.pt = triAddQueue.front()->ptB;
				splitPaneQueue.push(currPlane);

				currPlane.normal = GetSplitPlaneNormal(triAddQueue.front()->ptC, triAddQueue.front()->ptA, triAddQueue.front()->ptB);
				currPlane.pt = triAddQueue.front()->ptC;
				splitPaneQueue.push(currPlane);

			}
				


			std::vector<TriangleA3*> insideList; //Left
			std::vector<TriangleA3*> outsideList; //Right

			PlaneNode* currPlaneNode = nullptr;
			TriangleA3* currTri = nullptr;

			std::queue<PlaneNode*> planeQueue;
			planeQueue.push(rootNode);




			//To Do: Allow herustics

			//Auto partition
			size_t numAttempts = 10;
			while (!planeQueue.empty() && numAttempts > 0 && !splitPaneQueue.empty())
			{
				--numAttempts;

				insideList.clear();
				outsideList.clear();

				currPlaneNode = planeQueue.front();
				planeQueue.pop();
				
				while (!triAddQueue.empty())
				{
					currTri = triAddQueue.front();
					triAddQueue.pop();

					if (isInsidePlane(*currTri, splitPaneQueue.front().normal, splitPaneQueue.front().pt))
						insideList.push_back(currTri);
					else
						outsideList.push_back(currTri);
				}
				
				if (insideList.size() > triangleMaxLeaf)
				{
					currPlaneNode->leftInsideNode = new PlaneNode();
					nodeVector.push_back(rootNode->leftInsideNode);
					planeQueue.push(static_cast<PlaneNode*>(currPlaneNode->leftInsideNode));

					for (TriangleA3* tri : insideList)
						triAddQueue.push(tri);
				}
				else
				{
					currPlaneNode->leftInsideNode = new LeafNode();
					nodeVector.push_back(rootNode->leftInsideNode);

					for (TriangleA3* tri : insideList)
						static_cast<LeafNode*> (rootNode->leftInsideNode)->triVector.push_back(tri);
				}

				if (outsideList.size() > triangleMaxLeaf)
				{
					currPlaneNode->rightOutsideNode = new PlaneNode();
					nodeVector.push_back(rootNode->rightOutsideNode);
					planeQueue.push(static_cast<PlaneNode*>(currPlaneNode->rightOutsideNode));

					for (TriangleA3* tri : outsideList)
						triAddQueue.push(tri);
				}
				else
				{
					currPlaneNode->rightOutsideNode = new LeafNode();
					nodeVector.push_back(rootNode->rightOutsideNode);

					for (TriangleA3* tri : outsideList)
						static_cast<LeafNode*> (rootNode->rightOutsideNode)->triVector.push_back(tri);
				}

				splitPaneQueue.pop();

			}
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