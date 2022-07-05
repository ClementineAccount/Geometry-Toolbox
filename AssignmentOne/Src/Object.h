#pragma once 

#include "Assignment.h"
#include "mesh.h"
#include "BoundingVolume.h"

namespace Assignment
{
	class Object
	{
	public:

		Transform transform;
		std::string meshID;
		//Mesh objectMesh; 

		//What shader should this object be drawn with unless otherwise stated?
		std::string shaderID; 

		//If using the color shader --> what color to draw this with?
		glm::vec3 color; 

		//To Do: Add the bounding volume
		BV::BoundingVolume bv;
	};

	std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList, size_t start, size_t end, std::unordered_map<std::string, Mesh> const& loadedMeshMap);
	std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList, std::unordered_map<std::string, Mesh> const& loadedMeshMap);

	glm::mat4 calculateModel(Transform trans);
}
