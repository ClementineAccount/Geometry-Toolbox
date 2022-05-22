#include "SceneManager.h"
#include <utility>

bool SceneManager::MakeScene(std::string const& sceneName, std::unique_ptr<Scene> scenePtr)
{
	//To Do: add check for this successful or failed
	sceneMap.insert(std::make_pair(sceneName, std::move(scenePtr)));
	return true;
}
