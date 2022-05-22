#include "pch.h"
#include "CppUnitTest.h"
#include "../Assignment1/Src/Scene.h"
#include "../Assignment1/Src/Scene.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SceneTesting
{
	TEST_CLASS(SceneCreation)
	{
	public:
		
		TEST_METHOD(EmptyScene1)
		{
			Scenes::SceneFunctions::CreateEmptyScene();

			constexpr float durationSet = 10.0f;
			Scenes::SceneClass scene = Scenes::SceneFunctions::CreateEmptyScene(durationSet);
			Scenes::SceneTimer* timerPtr = static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0]);
			Assert::AreEqual(durationSet, timerPtr->sceneDuration);
		};

		TEST_METHOD(EmptyScene2)
		{
			Scenes::SceneFunctions::CreateEmptyScene();

			constexpr float durationSet = -23.0f;
			Scenes::SceneClass scene = Scenes::SceneFunctions::CreateEmptyScene(durationSet);
			Scenes::SceneTimer* timerPtr = static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0]);
			Assert::AreEqual(durationSet, timerPtr->sceneDuration);
		};

		TEST_METHOD(EmptyScene3)
		{
			Scenes::SceneFunctions::CreateEmptyScene();

			float durationSet = 0.0f;
			float durationAdd = 10.0f;

			Scenes::SceneClass scene = Scenes::SceneFunctions::CreateEmptyScene(durationSet);
			Scenes::SceneTimer* timerPtr = static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0]);
			timerPtr->elapsedDuration += durationAdd;


			Assert::AreEqual(durationSet + durationAdd, timerPtr->sceneDuration);
		};



	};
}
