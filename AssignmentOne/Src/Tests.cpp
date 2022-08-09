#include "Tests.h"
#include "BoundingVolume.h"
#include "OctTree.hpp"
#include "TriangleSoup.h"

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
			float fullLength = 10.0f;

			OctTree::Tree tree;
			tree.Init(pos, fullLength);

			tree.SplitCell(tree.rootNode);

			//Assumption is that we split in an anti-clockwise manner
			
			glm::vec3 topRightPos = glm::vec3(2.5f, 2.5f, 0.0f);
			glm::vec3 topLeftPos = glm::vec3(-2.5f, 2.5f, 0.0f);
			glm::vec3 bottomleftPos = glm::vec3(-2.5f, -2.5f, 0.0f);
			glm::vec3 bottomRightPos = glm::vec3(2.5f, -2.5f, 0.0f);


			//This tests that the children both exist and are created as expected

			assert(std::fabs(glm::length(tree.rootNode.children[0].centerPos - topRightPos)) < std::numeric_limits<float>::epsilon());
			assert(std::fabs(glm::length(tree.rootNode.children[1].centerPos - topLeftPos)) < std::numeric_limits<float>::epsilon());
			assert(std::fabs(glm::length(tree.rootNode.children[2].centerPos - bottomleftPos)) < std::numeric_limits<float>::epsilon());
			assert(std::fabs(glm::length(tree.rootNode.children[3].centerPos - bottomRightPos)) < std::numeric_limits<float>::epsilon());

			for (size_t i = 0; i < 4; ++i)
				assert(std::fabs(tree.rootNode.children[i].halfLength - tree.rootNode.halfLength * 0.5f) < std::numeric_limits<float>::epsilon());

			//Check if the recursion is accurate (on only one)

			//The top right is split again
			//tree.SplitCell(tree.rootNode.children[0]);
			//glm::vec3 topRightChild = glm::vec3(3.75f, 3.75f, 0.0f);
			//assert(std::fabs(glm::length(tree.rootNode.children[0].children[0].centerPos - topRightChild)) < std::numeric_limits<float>::epsilon());
		}


		void TestA3()
		{
			//TestOctTree2D();
			//TestOctTreeCell();
			//TestOctTreeSplit();
		}

		void TestAll()
		{
			TestPosMean();
			TestLargestSpread();
			TestRitters();

			TestA3();
		}

	}

}

