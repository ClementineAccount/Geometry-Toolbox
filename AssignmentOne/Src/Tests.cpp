#include "Tests.h"
#include "BoundingVolume.h"
#include "OctTree.hpp"
#include "TriangleSoup.h"
#include "BSP.h"


#include <limits>

namespace Assignment
{
	namespace Tests
	{
		void TestPosMean()
		{
			std::vector<glm::vec3> pos;
			pos.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
			pos.push_back(glm::vec3(20.0f, 10.0f, 10.0f));
			pos.push_back(glm::vec3(30.0f, 10.0f, 10.0f));

			glm::vec3 expectedPos = glm::vec3(20.0f, 10.0f, 10.0f);

			assert(expectedPos == BV::calculatePositionMean(pos));
		}

		void TestLargestSpread()
		{
			std::vector<glm::vec3> pos;
			pos.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
			pos.push_back(glm::vec3(100.0f, 10.0f, 10.0f));
			pos.push_back(glm::vec3(200.0f, 10.0f, 10.0f));

			glm::vec3 expected = worldRight;
			assert(expected == BV::axisOffsets::axisToDirection(BV::largestSpreadAxis(pos)));

			pos.clear();
			pos.push_back(glm::vec3(10.0f, 100.0f, 10.0f));
			pos.push_back(glm::vec3(1.0f, 1000.0f, 10.0f));
			pos.push_back(glm::vec3(2.0f, 100.0f, 10.0f));

			expected = worldUp;
			glm::vec3 result = BV::axisOffsets::axisToDirection(BV::largestSpreadAxis(pos));
			assert(expected == result);
		}


		void TestRitters()
		{
			BV::Sphere sphereBV;
			std::vector<glm::vec3> posVector;
			posVector.push_back(glm::vec3(-2.0f, 0.0f, 0.0f));
			posVector.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
			posVector.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
			posVector.push_back(glm::vec3(2.0f, 0.0f, 0.0f));

			sphereBV.CalculateRitters(posVector);

			glm::vec3 expectedCenter = glm::vec3(0.0f, 0.0f, 0.0f);
			float expectedRadius = 2.0f;

			assert(expectedCenter == sphereBV.centerPos);
			assert(expectedRadius == sphereBV.radius);
		}


		void TestOctTree2D()
		{
			// Test in 2D first
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			float fullLength = 10.0f;

			OctTree::Tree tree;
			tree.Init(pos, fullLength);

			TriangleA3 triA;
			//triA.ptA = glm::vec3(5.0f, 5.0f, 0.0f)
			tree.Insert(&triA);

			assert(tree.rootNode.triangleVector.front() == &triA);
		}

		//To Do: Rewrite it to use Quad

		void TestOctTreeCell()
		{
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			float fullLength = 10.0f;

			OctTree::Tree tree;
			tree.Init(pos, fullLength);

			glm::vec3 ptA = glm::vec3(5.0f, 5.0f, 0.0f);
			glm::vec3 ptB = glm::vec3(4.0f, 4.0f, 0.0f);
			glm::vec3 ptC = glm::vec3(-5.0f, -5.0f, 0.0f);
			glm::vec3 ptD = glm::vec3(-5.0f, 5.0f, 0.0f);
			glm::vec3 ptE = glm::vec3(5.0f, -5.0f, 0.0f);

			assert(tree.whichOct(tree.rootNode.centerPos, ptA) == OctTree::upRightQuad);
			assert(tree.whichOct(tree.rootNode.centerPos, ptB) == OctTree::upRightQuad);
			assert(tree.whichOct(tree.rootNode.centerPos, ptC) == OctTree::downLeftQuad);
			assert(tree.whichOct(tree.rootNode.centerPos, ptD) == OctTree::upLeftQuad);
			assert(tree.whichOct(tree.rootNode.centerPos, ptE) == OctTree::downRightQuad);
		}

		void TestOctTreeSplit()
		{
			//Test if the split is correct
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			float fullLength = 100.0f;

			size_t maxSize = 3;

			OctTree::Tree tree;
			tree.Init(pos, fullLength, maxSize);

			TriangleA3 A;
			A.ptA = glm::vec3(30, 30, 0.0f);

			TriangleA3 B;
			B.ptA = glm::vec3(30.0f, -30.0f, 0.0f);

			TriangleA3 C;
			C.ptA = glm::vec3(40.0f, -25.0f, 0.0f);

			tree.Insert(&A);
			tree.Insert(&B);
			tree.Insert(&C);

			//Check that there was NO split 
			assert(tree.rootNode.childMap.empty());
			assert(tree.currLevel == 0);


			auto checkAdded = [&](TriangleA3& tri)
			{
				return (tree.GetNodeWithObj(&tri) != nullptr);
			};

			//Check that the nodes were added
			assert(checkAdded(A));
			assert(checkAdded(B));
			assert(checkAdded(C));

			TriangleA3 D;
			D.ptA = glm::vec3(-30.0f, 30.0f, 0.0f);

			tree.Insert(&D);

			//Check that a split happened
			assert(tree.currLevel == 1);
			assert(tree.rootNode.childMap.size() == 8);

			//Check that the nodes were still here and added after a split
			assert(checkAdded(A));
			assert(checkAdded(B));
			assert(checkAdded(C));
			assert(checkAdded(D));

			TriangleA3 E;
			E.ptA = glm::vec3(-30.0f, -30.0f, 0.0f);

			tree.Insert(&E);

			//Check that a split did NOT happen
			assert(tree.currLevel == 1);
			assert(tree.rootNode.childMap.size() == 8);

			assert(checkAdded(A));
			assert(checkAdded(B));
			assert(checkAdded(C));
			assert(checkAdded(D));
			assert(checkAdded(E));

			TriangleA3 F;
			F.ptA = glm::vec3(33.0f, -33.0f, 0.0f); //Should be next to B

			tree.Insert(&F);

			//Check that a split did NOT happen
			assert(tree.currLevel == 1);
			assert(tree.rootNode.childMap.size() == 8);

			assert(checkAdded(A));
			assert(checkAdded(B));
			assert(checkAdded(C));
			assert(checkAdded(D));
			assert(checkAdded(E));
			assert(checkAdded(F));


			TriangleA3 G;
			G.ptA = glm::vec3(33.0f, -50.0f, 0.0f);

			tree.Insert(&G);
			assert(tree.currLevel == 2);
			assert(tree.rootNode.childMap.size() == 8);

			assert(checkAdded(A));
			assert(checkAdded(B));
			assert(checkAdded(C));
			assert(checkAdded(D));
			assert(checkAdded(E));
			assert(checkAdded(F));
			assert(checkAdded(G));

			//These two should share the same node
			assert(tree.GetNodeWithObj(&F) == tree.GetNodeWithObj(&B));
		}


		void TestSplitPlane()
		{

			glm::vec3 pt0 = glm::vec3(1.0f, 1.0f, 0.0f);
			glm::vec3 pt1 = glm::vec3(2.0f, 1.0f, 0.0f);
			glm::vec3 pt2 = glm::vec3(2.0f, 2.0f, 0.0f);
			glm::vec3 pt3 = glm::vec3(2.0f, -2.0f, 0.0f);

			glm::vec3 planeNormal = BSP::Tree::GetSplitPlaneNormal(pt0, pt1, pt2);
			assert(planeNormal == glm::vec3(0.0f, 1.0f, 0.0f));

			//Expected that pt2 is 'above' the split plane
			assert(glm::dot(pt2, planeNormal) > 0.0f);

			//Expected that pt1 and pt0 are 'on' the plane itself and hence close to perpendicular (or is)
			assert(glm::dot(pt1, planeNormal) >= 0.0f && glm::dot(pt0, planeNormal) >= 0.0f);

			//Expected that pt3 is 'below' the half plane on the outside.
			assert(glm::dot(pt3, planeNormal) < 0.0f);

			//Check that pt3 is still below even if the z-axis is different
			pt3.z = 100.0f;
			assert(glm::dot(pt3, planeNormal) < 0.0f);

		}


		void TestBSP()
		{



		}


		void TestA3()
		{
			//TestOctTree2D();
			//TestOctTreeCell();
			TestOctTreeSplit();
		}

		void TestAll()
		{
			TestPosMean();
			TestLargestSpread();
			TestRitters();

			TestSplitPlane();

			TestA3();
		}

	}

}

