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
			
			auto timer = SceneFunctions::getSceneData<Scenes::SceneTimer>(scene);

			//To Do: Possibly refactor that long cast to allow better generic function input output stuff?
			Assert::AreEqual(durationSet, timer.get()->sceneDuration);
		};

		TEST_METHOD(EmptyScene2)
		{
			Scenes::SceneFunctions::CreateEmptyScene();

			constexpr float durationSet = -23.0f;
			Scenes::SceneClass scene = Scenes::SceneFunctions::CreateEmptyScene(durationSet);
			auto timer = SceneFunctions::getSceneData<Scenes::SceneTimer>(scene);
			Assert::AreEqual(durationSet, timer.get()->sceneDuration);
		};

		TEST_METHOD(EmptyScene3)
		{
			Scenes::SceneFunctions::CreateEmptyScene();

			float durationSet = 0.0f;
			float durationAdd = 10.0f;

			Scenes::SceneClass scene = Scenes::SceneFunctions::CreateEmptyScene(durationSet);
			//Scenes::SceneTimer* timerPtr = static_cast<Scenes::SceneTimer*>(scene.sceneDataContainer[0]);
			//timerPtr->sceneDuration += durationAdd;
			auto timer = SceneFunctions::getSceneData<Scenes::SceneTimer>(scene);
			timer.get()->sceneDuration += durationAdd;

			Assert::AreEqual(durationSet + durationAdd, timer.get()->sceneDuration);
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
			constexpr float loopTick = 0.1f;
			float durationTime = 1.0f;

			using namespace Scenes;

			std::shared_ptr<SceneClass> defaultScene = std::make_shared<SceneClass>(SceneFunctions::CreateEmptyScene(durationTime));

			SceneManager sm;

			std::string sceneName = "TestUpdate";
			sm.addScene(sceneName, defaultScene);
			
			sm.runtimeScenes.push_back(std::move(defaultScene));
			while (durationTime > 0.0f)
			{
				sm.runScenes(loopTick);
				durationTime -= loopTick;
			}

			//Can't think of ways to improve the use of .get() everywhere
			auto const& scene = sm.getScene(sceneName);
			auto timer = SceneFunctions::getSceneData<SceneTimer>(*scene.get());

			Assert::IsTrue(timer.get()->elapsedDuration > durationTime);
		}
	};
}
