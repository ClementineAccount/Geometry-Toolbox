#include "Tests.h"
#include "BoundingVolume.h"

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

		void TestAll()
		{
			TestPosMean();
			TestLargestSpread();
		}

	}

}

