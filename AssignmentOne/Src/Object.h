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
		Mesh* objectMesh; 

		//What shader should this object be drawn with unless otherwise stated?
		std::string shaderID; 

		//If using the color shader --> what color to draw this with?
		glm::vec3 color; 

	};

	std::vector<glm::vec3> GetObjectPositions(Object const& obj);
	std::vector<glm::vec3> GetObjectPositions(std::vector<Object const*> const& objectList);
	std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList, size_t start, size_t end);
	std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList);

	glm::mat4 calculateModel(Transform trans);
}
