#pragma once 

#include "Scene.h"

#include <unordered_map>

//for smart ptrs
#include <memory> 

//constexpr std::string defaultSceneName = "NULL";

//To Do: better encapsulation if necessary with more time
//To Do: copy constructor if necssary
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


	bool MakeScene(std::string const& sceneName, std::unique_ptr<Scene> scenePtr);

private:
	//Access scene map via string key comparison
	std::unordered_map<std::string, std::unique_ptr<Scene>> sceneMap;
};
