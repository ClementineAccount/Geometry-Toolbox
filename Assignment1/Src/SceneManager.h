#pragma once 

#include "Scene.h"

#include <unordered_map>

//for smart ptrs
#include <memory> 

#include <queue>

//constexpr std::string defaultSceneName = "NULL";

//To Do: better encapsulation if necessary with more time
//To Do: copy constructor if necssary

namespace Scenes
{
	class SceneManager
	{
	public:

		SceneManager() {};
		~SceneManager() {};

		//Disallow copy constructor as it is not a feature
		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

		//SceneManager() {};
		//~SceneManager();

		//Using containers can allow possible multithreading support?

		void addScene(std::string const&& sceneName, std::shared_ptr<SceneClass> scenePtr);

		//Update the current runtime scenes
		void runScenes(float deltaTime = 0.0f);

	//To Do: Add encapsulation later
	public:
		//Access scene map via string key comparison
		std::unordered_map <std::string, std::shared_ptr<SceneClass>> sceneMap;

		//Queue of scenes that are currently in initialization phase
		std::queue<std::weak_ptr<SceneClass>> initScenes;

		//The scenes that are currently in its runtime loops
		std::vector<std::weak_ptr<SceneClass>> runtimeScenes;

		//Queue of scenes to be cleaned up. (Could this be returned to a caller who would handle it?)
		std::queue<std::weak_ptr<SceneClass>> cleanUpScenes;
	};
}

