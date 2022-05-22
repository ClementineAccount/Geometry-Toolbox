#pragma once 

#include <functional>
#include <vector>


namespace Scenes
{
	// Thought long and hard about a possible template implmentation of sceneData without polymorphic inheritance that can work with a standard
	// scene manager and std::containers simply but this seems to be the only way now.
	struct SceneData
	{
	public:
		  virtual ~SceneData() {};
	};

	struct SceneTimer : public SceneData 
	{
		SceneTimer() = default;

		float elapsedDuration = 0.0f;
		float sceneDuration = 10.0f;
	};

	struct SceneClass
	{
	public:

		bool isRuntime = true;

		//The input parameter is float to account for passing in deltaTime. 
		//The output is int to account for error returns
		//The functions are to be run in sequence, from lhs to rhs ([0] onwards)

		using sceneFunctionType = std::function<int(std::vector<SceneData*>&, float)>;
		using functionVector = typename std::vector<sceneFunctionType>;
		using dataContainerType = typename std::vector<SceneData*>;

		functionVector startupFunctions;

		functionVector runtimeFunctions;
		functionVector shutdownFunctions;

		//Allows some degree of polymorphism for dynamically serialized or allocated data for each scene in SceneManager
		std::vector<SceneData*> sceneDataContainer;
	};


	namespace SceneFunctions
	{
		SceneClass CreateEmptyScene(float sceneDuration = -1.0f);
	}

}


