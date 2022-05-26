#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AssignmentTests
{
	TEST_CLASS(AssignmentTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			GeometryToolbox::GLApplication app;
			app.initApplication();
		}
	};
}
