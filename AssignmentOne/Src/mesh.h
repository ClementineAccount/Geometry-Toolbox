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
	struct Vertices
	{
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> colors;
	};


	struct Indices
	{
		using indicesType = uint32_t;
		std::vector<indicesType> indexVector;
	};

	struct MeshBuffers
	{
		unsigned int VAO = 0;
		unsigned int VBO = 0;

		unsigned int arrayCount;

		unsigned char drawType = GL_TRIANGLES;

		bool isDrawElements = false;
		unsigned int elementCount;
		unsigned int EBO;
	};

	std::vector<GLfloat> verticesFromVectorList(std::vector<glm::vec3> vectorList);

	MeshBuffers initVBO(std::vector<GLfloat> meshPositions, std::vector<GLfloat> meshColor);


	MeshBuffers initBuffers(Vertices const& verticeSOA, Indices const& indices);

	struct Mesh
	{
	public:
		Vertices meshVertices;
		Indices meshIndices;

		MeshBuffers meshBuffer;

		void loadOBJ(std::string const& filePath) noexcept;
		void ProcessMesh(const aiMesh& addMesh, const aiScene& Scene) noexcept;
		void ProcessNode(const aiNode& Node, const aiScene& scene) noexcept;
	};
}

