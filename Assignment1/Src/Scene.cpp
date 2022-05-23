#include "Scene.h"
#include <iostream>

namespace Scenes
{
	//For unit testing, we can create a default scene
	SceneClass SceneFunctions::CreateEmptyScene(float sceneDuration)
	{
		SceneClass defaultScene;

		//Memory can be managed by the scene

		std::shared_ptr<SceneTimer> sceneTimerPtr = std::make_shared<SceneTimer>();
		sceneTimerPtr.get()->elapsedDuration = 0.0f;
		sceneTimerPtr.get()->sceneDuration = sceneDuration;

		defaultScene.sceneDataContainer.push_back(sceneTimerPtr);

		using dataContainerType = SceneClass::dataContainerType;
		dataContainerType dataContainer = defaultScene.sceneDataContainer;


		//To Do: Replace cout with a proper logger class
		SceneClass::sceneFunctionType f_initScene = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
		{
			std::cout << "f_initScene()\n";
			return 0;
		};

		defaultScene.startupFunctions.push_back(f_initScene);

		SceneClass::sceneFunctionType f_updateScene = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
		{
			auto timerPtr = std::static_pointer_cast<SceneTimer>(dataContainer[0]);

			timerPtr.get()->elapsedDuration += deltaTime;
			if (timerPtr.get()->elapsedDuration > timerPtr.get()->sceneDuration)
			{
				std::cout << "f_updateScene(): Scene ended.\n";
				return 1;
			}
			return 0;
		};

		defaultScene.runtimeFunctions.push_back(f_updateScene);

		SceneClass::sceneFunctionType f_endScene = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
		{

			std::cout << "f_endScene(): The scene has ended.\n";
			return 0;
		};

		defaultScene.shutdownFunctions.push_back(f_endScene);

		return defaultScene;
	}
}
