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
				positions.emplace_back( glm::vec3(currMesh->mVertices[j].x, currMesh->mVertices[j].y, currMesh->mVertices[j].z));
			}
		}
	}
}
