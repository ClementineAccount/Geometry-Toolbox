#include "AssignmentOne.h"
#include <iostream>

namespace AssignmentOne
{
	const std::string shaderFolderPath = "../Common/Shaders/";

	std::string vertexShaderPath = shaderFolderPath + "VertexShader.vert";
	std::string fragShaderPath = shaderFolderPath + "FragShader.frag";

	shaderFilePath defaultShader{ vertexShaderPath, fragShaderPath, "defaultShader" };

	glm::vec3 backgroundColor{ 0.0f, 0.0f, 0.0f };

	Mesh quadMesh;
	ShaderContainer assignmentShaders;


	std::vector<drawObject> drawList;


	Mesh InitQuadMesh()
	{
		std::vector<GLfloat> quadPositions =
		{
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		};

		constexpr float quadColorR = 0.0f;
		constexpr float quadColorG = 1.0f;
		constexpr float quadColorB = 0.85f;

		std::vector<GLfloat> quadColors =
		{
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
		};

		std::vector<GLfloat> quadVertices;
		quadVertices.insert(quadVertices.end(), std::make_move_iterator(quadPositions.begin()), std::make_move_iterator(quadPositions.end()));
		quadVertices.insert(quadVertices.end(), std::make_move_iterator(quadColors.begin()), std::make_move_iterator(quadColors.end()));


		//Create the VBO and VAOs

		Mesh quadMesh;

		glGenVertexArrays(1, &(quadMesh.VAO));
		glBindVertexArray(quadMesh.VAO);
		glGenBuffers(1, &(quadMesh.VBO));

		glBindBuffer(GL_ARRAY_BUFFER, quadMesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(GLfloat), quadVertices.data(), GL_STATIC_DRAW);

		const size_t offsetToFirstPos = 0; //in index before converted to bytes
		const size_t vertexCoordinateStride = 0;
		const size_t indexOfPosition = 0; //in the fragment shader
		const size_t numberPosCoordinatesPerVertex = 3;

		glEnableVertexAttribArray(indexOfPosition);
		glVertexAttribPointer(static_cast<GLuint>(indexOfPosition), static_cast<GLint>(numberPosCoordinatesPerVertex), GL_FLOAT, GL_FALSE, vertexCoordinateStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstPos));


		const size_t offsetToFirstColor = 18; //in index before converted to bytes
		const size_t vertexColorStride = 0;
		const size_t indexOfColor = 1; //in the fragment shader
		const size_t numberColorPerVertex = 3;

		glVertexAttribPointer(static_cast<GLuint>(indexOfColor), static_cast<GLint>(numberColorPerVertex), GL_FLOAT, GL_FALSE, vertexColorStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstColor));
		glEnableVertexAttribArray(indexOfColor);

		return quadMesh;
	}

	void SubmitDraw(Model const& model, Mesh const& mesh)
	{
		drawList.emplace_back(drawObject{ model, mesh });
	}

	void DrawAll(std::vector<drawObject> const& drawList)
	{
		for (drawObject const& currDraw : drawList)
		{
			//mvp
			glm::mat4 modelMat = glm::mat4(1.0f);
			glm::mat4 viewMat = glm::mat4(1.0f);
			glm::mat4 perspectiveMat = glm::mat4(1.0f);

			glm::mat4 mvpMat = glm::mat4(1.0f);
			mvpMat = perspectiveMat * viewMat * modelMat;

			//use default shader for now
			unsigned int programID = assignmentShaders.getShaderID(defaultShader.shaderName);
			glUseProgram(programID);

			GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");

			glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMat[0][0]);
			glBindVertexArray(drawList[0].mesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	int InitAssignment()
	{
		assignmentShaders.loadShader(defaultShader.shaderName, defaultShader);
		std::cout << "Assignment Shaders Loaded\n";

		//Create cube
		quadMesh = InitQuadMesh();
		


		return 0;
	}

	//Temp values for testing
	int RenderQuadTest()
	{
		unsigned int programID = assignmentShaders.getShaderID(defaultShader.shaderName);
		glUseProgram(programID);

		// Uniform matrix
		// Uniform transformation (vertex shader)
		GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");

		// Draw the triangle !
		// T * R * S * Vertex
		glm::mat4 modelMat = glm::mat4(1.0f);
		glm::vec3 scaleVector = glm::vec3(1.0f);
		glm::vec3 centroid = glm::vec3(0.0f, -0.0f, 0.0f);
		glm::vec3 triPos = glm::vec3(0.0f, -0.5f, -1.0f);

		glm::vec3 quadPos{ 0.0f, 0.0f, 0.0f };
		glm::vec3 quadScale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 quadAngles{ 0.0f, 0.0f, 0.0f };

		modelMat = glm::translate(modelMat, quadPos);

		float pivotPercentFromLeft = 0.5f;

		modelMat = glm::translate(modelMat, glm::vec3(pivotPercentFromLeft * quadScale.x,
			pivotPercentFromLeft * quadScale.y,
			pivotPercentFromLeft * quadScale.z));

		modelMat = glm::rotate(modelMat, quadAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMat = glm::rotate(modelMat, quadAngles.y, glm::vec3(0.0f, 1.0f, 1.0f));
		modelMat = glm::rotate(modelMat, quadAngles.x, glm::vec3(1.0f, 0.0f, 1.0f));
		modelMat = glm::translate(modelMat, glm::vec3(-pivotPercentFromLeft * quadScale.x, -pivotPercentFromLeft * quadScale.y, -pivotPercentFromLeft * quadScale.z));

		modelMat = glm::scale(modelMat, quadScale);

		glm::mat4 perspectiveMat = glm::mat4(1.0f);
		glm::mat4 viewMat = glm::mat4(1.0f);

		glm::mat4 mvpMat = glm::mat4(1.0f);
		mvpMat = perspectiveMat * viewMat * modelMat;

		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMat[0][0]);

		glBindVertexArray(quadMesh.VAO);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		//glDisableVertexAttribArray(0);

		return 0;
	}

	void RenderAssignment()
	{
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		SubmitDraw(Model{}, quadMesh);
		DrawAll(drawList);
		drawList.clear();
	}

}
