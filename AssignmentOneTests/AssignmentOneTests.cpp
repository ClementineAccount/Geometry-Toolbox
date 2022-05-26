#include "pch.h"
#include "CppUnitTest.h"

#include "../Assignment1/Src/GLApplication.h"
#include "../Assignment1/Src/GlApplication.cpp"

// Include Dear Imgui
//#include "../imgui/imgui.h"
//#include "../imgui/imgui_impl_glfw.h"
//#include "../imgui/imgui_impl_opengl3.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AssignmentOneTests
{
	TEST_CLASS(AssignmentOneTests)
	{
	public:
		
		TEST_METHOD(GLAppicationInit)
		{
			GeometryToolbox::GLApplication app;
			//app.initApplication();

			//Assert::IsNotNull(app.window, L"Window should have been initialized");
			//app.shutdownApplication();
		}
	};
}
