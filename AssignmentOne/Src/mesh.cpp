#pragma once 


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "GLApplication.h"
#include <glm/glm.hpp>



#include <vector>
#include <assert.h>

namespace Assignment
{
	void Mesh::loadOBJ(std::string const& filePath)
	{
		//ctrlc ctrlv: https://learnopengl.com/Model-Loading/Model
		Assimp::Importer importer;
		const aiScene* meshScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

		//To Do: Replace this with proper runtime error handling for Release mode
		assert(meshScene->mRootNode && ("failed to load the mesh at: " + filePath).c_str());
		assert(meshScene->mMeshes && "no mesh?");

		//What if there are child meshes? You could add them to a list using bfs or dfs but we will do that later

		//"Assimp calls their vertex position array mVertices which isn't the most intuitive name."
		for (size_t i = 0; i < meshScene->mNumMeshes; ++i)
		{
			aiMesh* currMesh = meshScene->mMeshes[i];
			for (size_t j = 0; j < currMesh->mNumVertices; ++j)
			{
				meshVertices.positions.emplace_back( glm::vec3(currMesh->mVertices[j].x, currMesh->mVertices[j].y, currMesh->mVertices[j].z));
			}
		}
	}

	//Very similar to 'initVBO' from the assignment file (and will eventually be its replacement)
	void createBuffers(MeshBuffers& meshBuffer, Vertices const& vertices, Indices const& indices)
	{
		using vertexType = GLfloat;

		std::vector<vertexType> verticesList;

		//Add positions first 
		for (glm::vec3 const& pos : vertices.positions)
		{
			verticesList.emplace_back(pos.x);
			verticesList.emplace_back(pos.y);
			verticesList.emplace_back(pos.z);
		}

		//To do: The other vertices when

		glCreateBuffers(1, &meshBuffer.VBO);
		glNamedBufferStorage(meshBuffer.VBO, sizeof(vertexType) * vertices.positions.size(), verticesList.data(), GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &meshBuffer.VAO);
		glVertexArrayVertexBuffer(meshBuffer.VAO, 0, meshBuffer.VBO, 0, sizeof(vertexType));

		const size_t offsetToFirstPos = 0; //in index before converted to bytes
		const size_t vertexCoordinateStride = 0;
		const size_t indexOfPosition = 0; //in the fragment shader (Note
		const size_t numberPosCoordinatesPerVertex = 3;

		glEnableVertexAttribArray(indexOfPosition);
		glVertexAttribPointer(static_cast<GLuint>(indexOfPosition), 
			static_cast<GLint>(numberPosCoordinatesPerVertex), 
			GL_FLOAT, GL_FALSE, vertexCoordinateStride * sizeof(vertexType), (void*)(sizeof(vertexType) * offsetToFirstPos));


		glCreateBuffers(1, &meshBuffer.EBO);
		glNamedBufferStorage(meshBuffer.EBO, sizeof(Indices::indicesType) * indices.indexVector.size(), indices.indexVector.data(), GL_DYNAMIC_STORAGE_BIT);

	}

}
