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
	struct MeshBuffers
	{
		unsigned int VAO = 0;
		unsigned int VBO = 0;

		unsigned int arrayCount;

		GLchar drawType = GL_TRIANGLES;

		bool isDrawElements = false;
		unsigned int elementCount;
		unsigned int EBO;
	};

	//structure of arrays
	struct Vertices
	{
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> colors;
	};

	struct Indices
	{
		using indicesType = uint32_t;
		std::vector<indicesType> indexVector;
	};

	class Mesh
	{
	public:

		Vertices meshVertices;

		MeshBuffers meshBuffers;

		//Load assimap as a scene file
		//extract the vertices
		//allow create vbo
		void loadOBJ(std::string const& filePath);
	};

	void createBuffers(MeshBuffers& buffers, Vertices const& vertices, Indices const& indices);
}

