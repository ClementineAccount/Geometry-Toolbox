#include "Tests.h"
#include "BoundingVolume.h"
#include "OctTree.hpp"

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

			OctTree::Tree<glm::vec3> tree;
			tree.Init(pos, fullLength);

			glm::vec3 ptA = glm::vec3(5.0f, 5.0f, 0.0f);
			tree.Insert(ptA);

			assert(tree.rootNode->objectVector.front() == ptA);
		}


		void TestA3()
		{
			TestOctTree2D();
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

