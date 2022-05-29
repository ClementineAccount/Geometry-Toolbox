#include "SceneManager.h"
#include <utility>
#include <assert.h>

using namespace Scenes;

void SceneManager::addScene(std::string const& sceneName, std::shared_ptr<SceneClass> scenePtr)
{
	//assert(sceneMap.count(sceneName) == 0 && "AssignmentScenes with name already exists.");
	sceneMap.insert({ sceneName, scenePtr });
	initSceneQueue.push(scenePtr);
}

void SceneManager::initScenes(float deltaTime)
{
	while (!initSceneQueue.empty())
	{
		std::shared_ptr<SceneClass> currScene = initSceneQueue.front().lock();
		SceneFunctions::LoopFunctions(currScene->startupFunctions, currScene->sceneDataContainer, deltaTime);
		runtimeScenesVector.push_back(currScene);
		initSceneQueue.pop();
	}
}

void SceneManager::runScenes(float deltaTime)
{
	for (std::weak_ptr<SceneClass> scene : runtimeScenesVector)
	{
		SceneFunctions::LoopFunctions(scene.lock()->runtimeFunctions, scene.lock()->sceneDataContainer, deltaTime);
	}
}

SceneManager::SceneReferenceTypeMap SceneManager::getScene(std::string const& sceneKey)
{
	return sceneMap.at(sceneKey);
}

