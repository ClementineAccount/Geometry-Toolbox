#pragma once 

#include "Assignment.h"
#include "mesh.h"


namespace Assignment
{
	class Object
	{
	public:

		Transform transform;
		Mesh objectMesh; 

		//What shader should this object be drawn with unless otherwise stated?
		std::string shaderID; 

		//If using the color shader --> what color to draw this with?
		glm::vec3 color; 

		//To Do: Add the bounding volume
	};

	glm::mat4 calculateModel(Transform trans);
}
