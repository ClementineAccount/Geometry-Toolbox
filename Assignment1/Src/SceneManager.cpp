#include "SceneManager.h"
#include <utility>
#include <assert.h>

using namespace Scenes;

void SceneManager::addScene(std::string const& sceneName, std::shared_ptr<SceneClass> scenePtr)
{
	//assert(sceneMap.count(sceneName) == 0 && "Scene with name already exists.");
	sceneMap.insert({ sceneName, scenePtr });
	initScenes.push(scenePtr);
}

void SceneManager::runScenes(float deltaTime)
{
	for (auto const& scene : runtimeScenes)
	{
		for (SceneClass::sceneFunctionType func : scene.lock().get()->runtimeFunctions)
		{
			func(scene.lock().get()->sceneDataContainer, deltaTime);
		}
	}
}

SceneManager::SceneReferenceTypeMap SceneManager::getScene(std::string const& sceneKey)
{
	return sceneMap.at(sceneKey);
}

