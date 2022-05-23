#include "pch.h"
#include "CppUnitTest.h"
#include "../Assignment1/Src/Scene.h"
#include "../Assignment1/Src/Scene.cpp"
#include "../Assignment1/Src/SceneManager.h"
#include "../Assignment1/Src/SceneManager.cpp"

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
			//Scenes::SceneTimer* timerPtr = static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0]);
			
			//To Do: Possibly refactor that long cast to allow better generic function input output stuff?
			Assert::AreEqual(durationSet, static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0].get())->sceneDuration);
		};

		TEST_METHOD(EmptyScene2)
		{
			Scenes::SceneFunctions::CreateEmptyScene();

			constexpr float durationSet = -23.0f;
			Scenes::SceneClass scene = Scenes::SceneFunctions::CreateEmptyScene(durationSet);
			Assert::AreEqual(durationSet, static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0].get())->sceneDuration);
		};

		TEST_METHOD(EmptyScene3)
		{
			Scenes::SceneFunctions::CreateEmptyScene();

			float durationSet = 0.0f;
			float durationAdd = 10.0f;

			Scenes::SceneClass scene = Scenes::SceneFunctions::CreateEmptyScene(durationSet);
			//Scenes::SceneTimer* timerPtr = static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0]);
			//timerPtr->sceneDuration += durationAdd;
			static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0].get())->sceneDuration += 10.0f;

			Assert::AreEqual(durationSet + durationAdd, static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0].get())->sceneDuration);
		};
	};

	TEST_CLASS(SceneManagement)
	{
	public:
		TEST_METHOD(SceneManagementAdd)
		{
			using namespace Scenes;

			std::shared_ptr<SceneClass> defaultScene = std::make_shared<SceneClass>(SceneFunctions::CreateEmptyScene());

			SceneManager sm;
			
			std::string sceneName = "Default Scene";
			sm.addScene(std::move(sceneName), defaultScene);

			Assert::IsTrue(defaultScene == sm.initScenes.front().lock());
		}

		TEST_METHOD(SceneManagementUpdate)
		{
			//We simulate a tick per loop
			constexpr float loopTick = 0.01f;
			float durationTime = 5.0f;

			using namespace Scenes;

			std::shared_ptr<SceneClass> defaultScene = std::make_shared<SceneClass>(SceneFunctions::CreateEmptyScene(durationTime));

			SceneManager sm;

			std::string sceneName = "Default Scene";
			//sm.addScene(std::move(sceneName), defaultScene);
			
			sm.runtimeScenes.push_back(std::move(defaultScene));
			while (durationTime > 0.0f)
			{
				sm.runScenes(loopTick);
				durationTime -= loopTick;
			}
			Assert::IsTrue(static_cast<Scenes::SceneTimer*>(defaultScene.get()->sceneDataContainer[0].get())->elapsedDuration > durationTime);
		}
	};
}
