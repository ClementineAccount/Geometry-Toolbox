#include "mesh.h"
#include "TriangleSoup.h"


namespace Assignment
{
	//To Do: Add check if the buffer doesn't exist yet to not delete
	void TriangleSoup::ClearBuffers()
	{
		glDeleteVertexArrays(1, &meshBuffers.VAO);
		glDeleteBuffers(1, &meshBuffers.VBO);
		glDeleteBuffers(1, &meshBuffers.EBO);
	}

	void TriangleSoup::UpdateBuffers()
	{
		int count = -1;
		for (TriangleA3 const& tri : triangleList)
		{
			vertices.colors.push_back(tri.color);
			vertices.colors.push_back(tri.color);
			vertices.colors.push_back(tri.color);
			
			vertices.positions.push_back(tri.ptA);

			//Just for testing, want to keep the normals consistent. Will remove afterwards
			vertices.normals.push_back(tri.ptA);
			indices.indexVector.push_back(++count);

			vertices.positions.push_back(tri.ptB);
			vertices.normals.push_back(tri.ptB);

			indices.indexVector.push_back(++count);

			vertices.positions.push_back(tri.ptC);
			vertices.normals.push_back(tri.ptC);

			indices.indexVector.push_back(++count);
		}
		meshBuffers = initBuffers(vertices, indices);
	}

}