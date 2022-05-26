#pragma once 

#include <functional>
#include <vector>
#include <memory>

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
		using dataContainerType = typename std::vector<std::shared_ptr<SceneData>>;

		using sceneFunctionReturnType = int;
		using sceneFunctionType = std::function<sceneFunctionReturnType(dataContainerType&, float)>;
		using functionVector = typename std::vector<sceneFunctionType>;
		
		SceneClass() = default;
		SceneClass(dataContainerType& _dataContainer) : sceneDataContainer(_dataContainer) {};

		//bool isRuntime = true;

		//The input parameter is float to account for passing in deltaTime. 
		//The output is int to account for error returns
		//The functions are to be run in sequence, from lhs to rhs ([0] onwards)

		dataContainerType sceneDataContainer;

		functionVector startupFunctions;
		functionVector runtimeFunctions;
		functionVector shutdownFunctions;

		//Allows some degree of polymorphism for dynamically serialized or allocated data for each scene in SceneManager

	};


	namespace SceneFunctions
	{
		SceneClass CreateTestScene(float sceneDuration = -1.0f);

		void LoopFunctions(SceneClass::functionVector& functionList, SceneClass::dataContainerType& data, float deltaTime = 0.0f);

		template <typename T>
		std::shared_ptr<T> getSceneData(SceneClass const& sceneRef, size_t index = 0)
		{
			return std::dynamic_pointer_cast<T>(sceneRef.sceneDataContainer[index]);
		}
	}

}


