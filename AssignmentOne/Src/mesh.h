#pragma once
#include <string>
#include "GLApplication.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include <glm/glm.hpp>

//To Do: Add the assimap support

namespace Assignment
{

	class Mesh
	{
	public:

		//structure of arrays
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;

		//Load assimap as a scene file
		//extract the vertices
		//allow create vbo
		void loadOBJ(std::string const& filePath);

	};


	namespace Tests
	{

	}
}

