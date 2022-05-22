#include "Scene.h"
#include <iostream>

namespace Scenes
{
	//For unit testing, we can create a default scene
	SceneClass SceneFunctions::CreateEmptyScene(float sceneDuration)
	{
		SceneClass defaultScene;

		//Memory can be managed by the scene
		SceneTimer* duration = new SceneTimer();
		duration->elapsedDuration = 0.0f;
		duration->sceneDuration = sceneDuration;

		defaultScene.sceneDataContainer.push_back(duration);
		std::vector<SceneData*>& dataContainer = defaultScene.sceneDataContainer;

		using dataContainerType = SceneClass::dataContainerType;

		//To Do: Replace cout with a proper logger class
		SceneClass::sceneFunctionType f_initScene = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
		{
			std::cout << "f_initScene()\n";
			return 0;
		};

		defaultScene.startupFunctions.push_back(f_initScene);

		SceneClass::sceneFunctionType f_updateScene = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
		{
			SceneTimer* timerPtr = static_cast<SceneTimer*> (dataContainer[0]);

			timerPtr->elapsedDuration += deltaTime;
			if (timerPtr->elapsedDuration > timerPtr->sceneDuration)
			{
				std::cout << "f_updateScene(): Scene ended.\n";
				return 1;
			}
			return 0;
		};

		defaultScene.runtimeFunctions.push_back(f_updateScene);

		SceneClass::sceneFunctionType f_endScene = [](dataContainerType& dataContainer, float deltaTime = 0.0f)
		{
			SceneTimer* timerPtr = static_cast<SceneTimer*> (dataContainer[0]);
			delete timerPtr;

			std::cout << "f_endScene(): The scene has ended.\n";
			return 0;
		};

		defaultScene.shutdownFunctions.push_back(f_endScene);

		return defaultScene;
	}
}
