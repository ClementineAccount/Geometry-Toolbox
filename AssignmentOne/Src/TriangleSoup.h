#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include "mesh.h"
#include "Transform.h"

namespace Assignment
{
	//Triangle for BSP and Octree calculations. Models will need their triangles decomposed to this too
	struct TriangleA3
	{
		//Worldspace positon coordinates
		glm::vec3 ptA;
		glm::vec3 ptB;
		glm::vec3 ptC;

		//for rendering in triangle soup
		glm::vec3 color; 

		//void Init(glm::vec3 const& ptA, glm::vec3 const& ptB, glm::vec3 const& ptC);

		void SetColor(glm::vec3 setColor)
		{
			color = setColor;
		}
	};

	class TriangleSoup
	{
	public:
		~TriangleSoup() { ClearBuffers(); };

		std::vector<TriangleA3> triangleList;
		Vertices vertices;
		Indices indices;

		MeshBuffers meshBuffers;

		//unsigned int shaderID;

		void ClearBuffers();
		void UpdateBuffers();
	};

	}



