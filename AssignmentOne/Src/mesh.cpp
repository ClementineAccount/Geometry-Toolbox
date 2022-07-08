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
	std::vector<GLfloat> verticesFromVectorList(std::vector<glm::vec3> vectorList)
	{
		std::vector<GLfloat> vertices;
		for (auto& point : vectorList)
		{
			vertices.push_back(point.x);
			vertices.push_back(point.y);
			vertices.push_back(point.z);
		}

		return vertices;
	}

	//Call the buffers for sometihng meant to be drawn with GL_ARRAYS
	MeshBuffers initVBO(std::vector<GLfloat> meshPositions, std::vector<GLfloat> meshColor)
	{
		std::vector<GLfloat> vertices;
		vertices.insert(vertices.end(), std::make_move_iterator(meshPositions.begin()), std::make_move_iterator(meshPositions.end()));
		vertices.insert(vertices.end(), std::make_move_iterator(meshColor.begin()), std::make_move_iterator(meshColor.end()));

		MeshBuffers mesh;

		glGenVertexArrays(1, &(mesh.VAO));
		glBindVertexArray(mesh.VAO);
		glGenBuffers(1, &(mesh.VBO));

		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

		const size_t offsetToFirstPos = 0; //in index before converted to bytes
		const size_t vertexCoordinateStride = 0;
		const size_t indexOfPosition = 0; //in the fragment shader
		const size_t numberPosCoordinatesPerVertex = 3;

		glEnableVertexAttribArray(indexOfPosition);
		glVertexAttribPointer(static_cast<GLuint>(indexOfPosition), static_cast<GLint>(numberPosCoordinatesPerVertex), GL_FLOAT, GL_FALSE, vertexCoordinateStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstPos));

		const size_t offsetToFirstColor = meshPositions.size(); //in index before converted to bytes (6 times 3)
		const size_t vertexColorStride = 0;
		const size_t indexOfColor = 1; //in the fragment shader
		const size_t numberColorPerVertex = 3;

		glVertexAttribPointer(static_cast<GLuint>(indexOfColor), static_cast<GLint>(numberColorPerVertex), GL_FLOAT, GL_FALSE, vertexColorStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstColor));
		glEnableVertexAttribArray(indexOfColor);

		mesh.arrayCount = meshPositions.size();
		GLchar defaultDrawType = GL_TRIANGLES;
		mesh.drawType = defaultDrawType;

		return mesh;
	}

	MeshBuffers initBuffers(Vertices const& verticeSOA, Indices const& indices)
	{
		std::vector<GLfloat> vertices;

		std::vector<GLfloat> positions = verticesFromVectorList(verticeSOA.positions);
		vertices.insert(vertices.end(), positions.begin(), positions.end());

		std::vector<GLfloat> normals = verticesFromVectorList(verticeSOA.normals);
		vertices.insert(vertices.end(), normals.begin(), normals.end());

		std::vector<GLfloat> colors = verticesFromVectorList(verticeSOA.colors);
		vertices.insert(vertices.end(), colors.begin(), colors.end());


		MeshBuffers mesh;

		glGenVertexArrays(1, &(mesh.VAO));
		glBindVertexArray(mesh.VAO);
		glGenBuffers(1, &(mesh.VBO));

		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

		const size_t offsetToFirstPos = 0; //in index before converted to bytes
		const size_t vertexCoordinateStride = 0;
		const size_t indexOfPosition = 0; //in the vertex shader
		const size_t numberPosCoordinatesPerVertex = 3;

		glEnableVertexAttribArray(indexOfPosition);
		glVertexAttribPointer(static_cast<GLuint>(indexOfPosition), static_cast<GLint>(numberPosCoordinatesPerVertex), GL_FLOAT, GL_FALSE, vertexCoordinateStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstPos));

		const size_t offsetToFirstNormal = positions.size(); //in index before converted to bytes (6 times 3)
		const size_t vertexNormalStride = 0;
		const size_t indexOfNormal = 1; //in the vertex shader
		const size_t numNormalsPer = 3;

		glVertexAttribPointer(static_cast<GLuint>(indexOfNormal), static_cast<GLint>(numNormalsPer), GL_FLOAT, GL_FALSE, vertexNormalStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstNormal));
		glEnableVertexAttribArray(indexOfNormal);

		const size_t offsetToFirstColor = normals.size() + positions.size(); //in index before converted to bytes (6 times 3)
		const size_t vertexColorStride = 0;
		const size_t indexOfColor = 2; //in the vertex shader
		const size_t numberColorPerVertex = 3;

		glVertexAttribPointer(static_cast<GLuint>(indexOfColor), static_cast<GLint>(numberColorPerVertex), GL_FLOAT, GL_FALSE, vertexColorStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstColor));
		glEnableVertexAttribArray(indexOfColor);

		//Now do EBO

		//std::vector<int> indices;

		glGenBuffers(1, &mesh.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.indexVector.size() * sizeof(Indices::indicesType	), indices.indexVector.data(), GL_STATIC_DRAW);

		mesh.elementCount = indices.indexVector.size();
		mesh.isDrawElements = true;
		mesh.drawType = GL_TRIANGLES; //So we can do texture stuff with this later
		return mesh;
	}

	void Mesh::ProcessMesh(const aiMesh& addMesh, const aiScene& Scene) noexcept
	{

		//Go through the vertices to add positions, normals and so on
		for (auto i = 0u; i < addMesh.mNumVertices; ++i)
		{
			meshVertices.positions.emplace_back(addMesh.mVertices[i].x, addMesh.mVertices[i].y, addMesh.mVertices[i].z);

			//Add if there are normals
			if (addMesh.HasNormals())
			{
				meshVertices.normals.emplace_back(glm::vec3(addMesh.mNormals[i].x, addMesh.mNormals[i].y, addMesh.mNormals[i].z));
			}

			//if (addMesh.HasTextureCoords(0))
			//{
			//	mesh.meshVertices.textureCords.emplace_back(glm::vec2(addMesh.mTextureCoords[0][i].x, addMesh.mTextureCoords[0][i].y));
			//}

			//if (addMesh.HasNormals())
			//{
			//	mesh.meshVertices.normals.emplace_back(addMesh.mNormals[i].x, addMesh.mNormals[i].y, addMesh.mNormals[i].z);
			//}

			//if (addMesh.HasTangentsAndBitangents())
			//{
			//	mesh.meshVertices.tangent.emplace_back(addMesh.mTangents[i].x, addMesh.mTangents[i].y, addMesh.mTangents[i].z);
			//	mesh.meshVertices.biTangent.emplace_back(addMesh.mBitangents[i].x, addMesh.mBitangents[i].y, addMesh.mBitangents[i].z);
			//}

			if (addMesh.HasVertexColors(0))
			{
				//Only add the first color set
				meshVertices.colors.emplace_back(glm::vec3(addMesh.mColors[0][i].r, addMesh.mColors[0][i].g, addMesh.mColors[0][i].b));
			}
			else
			{
				static glm::vec3 defaultColor = glm::vec3(0.816, 0.949, 0.992);
				meshVertices.colors.push_back(defaultColor);
			}
		}

		//Go through the indices
		for (auto i = 0u; i < addMesh.mNumFaces; ++i)
		{
			const auto& Face = addMesh.mFaces[i];

			for (auto j = 0u; j < Face.mNumIndices; ++j)
				meshIndices.indexVector.push_back(Face.mIndices[j]);
		}
	}

	void Mesh::ProcessNode(const aiNode& Node, const aiScene& Scene) noexcept
	{
		for (auto i = 0u, end = Node.mNumMeshes; i < end; ++i)
		{
			aiMesh* pMesh = Scene.mMeshes[Node.mMeshes[i]];
			ProcessMesh(*pMesh, Scene);
		}

		for (auto i = 0u; i < Node.mNumChildren; ++i)
		{
			ProcessNode(*Node.mChildren[i], Scene);
		}
	}

	void Mesh::loadOBJ(std::string const& filePath) noexcept
	{
		Assimp::Importer importer;
		const aiScene* meshScene = importer.ReadFile(filePath
			, aiProcess_Triangulate                // Make sure we get triangles rather than nvert polygons
			| aiProcess_GenUVCoords                // Convert any type of mapping to uv mapping
			| aiProcess_TransformUVCoords          // preprocess UV transformations (scaling, translation ...)
			| aiProcess_FindInstances              // search for instanced meshes and remove them by references to one master
			| aiProcess_PreTransformVertices       // pre-transform all vertices
		);

		//To Do: Replace this with proper runtime error handling for Release mode
		assert(meshScene->mRootNode && ("failed to load the mesh at: " + filePath).c_str());
		assert(meshScene->mMeshes && "no mesh?");

		ProcessNode(*meshScene->mRootNode, *meshScene);

		meshBuffer = initBuffers(meshVertices, meshIndices);
	}
}
