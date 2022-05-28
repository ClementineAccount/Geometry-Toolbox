#include "AssignmentOne.h"
#include "AssignmentOneSettings.h"

#include <iostream>

// Include Dear Imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


//Settings
namespace AssignmentOne
{
	const std::string shaderFolderPath = "../Common/Shaders/";

	static std::string vertexShaderPath = shaderFolderPath + "VertexShader.vert";

	static std::string fragShaderPath = shaderFolderPath + "FragShader.frag";

	static shaderFilePath defaultShader{ vertexShaderPath, fragShaderPath, "defaultShader" };

	static glm::vec3 backgroundColor{ 0.0f, 0.0f, 0.0f };

	constexpr glm::vec3 planeScale = { 1000.0f, 1.0f, 1000.0f };
	glm::vec3 floorPlaneScale = { 1000.0f, 1.0f, 100.0f };

	constexpr float floorSpawnPos = -1.0f;

	//Cannot store as glm::vec3 because 'Right' might not mean x-axis
	constexpr float axisScaleRight = 100.0f;
	constexpr float axisScaleUp = 100.0f;
	constexpr float axisScaleForward = 100.0f;

	glm::vec3 axisColorRight{ 1.0f, 0.0f, 0.0f };
	glm::vec3 axisColorUp{ 0.0f, 1.0f, 0.0f };
	glm::vec3 axisColorForward{ 0.0f, 0.0f, 1.0f };


	namespace MeshNames
	{
		//can be used for planes too
		const char quad[] = "quad";
		const char axis[] = "axis";
	}

	namespace ModelNames
	{
		const char floorPlane[] = "floorPlaneModel";
	}

}




namespace AssignmentOne
{

	std::vector<GLfloat> forwardQuadVertexPos =
	{
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	std::vector<GLfloat> upQuadPositions =
	{
		-1.0f, 0.0f,  1.0f,  // 0
		 1.0f, 0.0f,  1.0f,  // 1
		 1.0f, 0.0f, -1.0f,  // 2
		 1.0f, 0.0f, -1.0f,  // 3
		-1.0f, 0.0f,  1.0f, // 4
		-1.0f, 0.0f, -1.0f  // 5
	};


	std::unordered_map<std::string, Mesh> meshMap;
	std::unordered_map<std::string, Model> modelMap;

	Mesh quadMesh;
	ShaderContainer assignmentShaders;

	//Default camera
	constexpr Camera defaultLookAtCamera;


	Camera currCamera = defaultLookAtCamera;

	//Yea its ugly but just set it in the main. I'll abstract it away later
	GeometryToolbox::GLApplication* applicationPtr;

	void setApplicationPtr(GeometryToolbox::GLApplication& appPtr)
	{
		applicationPtr = &appPtr;
	}

	std::vector<drawObject> drawList;


	//Call the buffers for sometihng meant to be drawn with GL_ARRAYS
	Mesh initVBOArrays(std::vector<GLfloat> meshPositions, std::vector<GLfloat> meshColor)
	{
		std::vector<GLfloat> vertices;
		vertices.insert(vertices.end(), std::make_move_iterator(meshPositions.begin()), std::make_move_iterator(meshPositions.end()));
		vertices.insert(vertices.end(), std::make_move_iterator(meshColor.begin()), std::make_move_iterator(meshColor.end()));

		Mesh mesh;

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

		return mesh;
	}

	//Using GL lines
	Mesh InitAxis()
	{
		glm::vec3 rightAxis = worldOrigin + (worldRight * axisScaleRight);
		glm::vec3 upAxis = worldOrigin + (worldUp * axisScaleUp);
		glm::vec3 forwardAxis = worldOrigin + (worldForward * axisScaleForward);

		//Why not just have a Line mesh and then transform three of them? Becuase I still need an axis to debug coordinate space

		std::vector<GLfloat> axisPoints
		{
			worldOrigin.x, worldOrigin.y, worldOrigin.z,
			rightAxis.x, rightAxis.y, rightAxis.z,

			worldOrigin.x, worldOrigin.y, worldOrigin.z,
			upAxis.x, upAxis.y, upAxis.z,

			worldOrigin.x, worldOrigin.y, worldOrigin.z,
			forwardAxis.x, forwardAxis.y, forwardAxis.z
		};

		std::vector<GLfloat> axisColor
		{
			axisColorRight.r, axisColorRight.g, axisColorRight.b,
			axisColorRight.r, axisColorRight.g, axisColorRight.b,

			axisColorUp.r, axisColorUp.g, axisColorUp.b,
			axisColorUp.r, axisColorUp.g, axisColorUp.b,

			axisColorForward.r,	axisColorForward.g, axisColorForward.b,
			axisColorForward.r,	axisColorForward.g, axisColorForward.b,
		};

		Mesh axisMesh = initVBOArrays(axisPoints, axisColor);
		axisMesh.drawType = GL_LINES;
		return axisMesh;
	}

	Mesh InitQuadMesh(std::vector<GLfloat>& quadPositions, float quadScale)
	{
		//Allows me to set quad positions normalized to 1 unit while applying the 0.5 scale afterwards
		for (GLfloat& i : quadPositions)
			i *= quadScale;

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

		Mesh quadMesh = initVBOArrays(quadPositions, quadColors);
		return quadMesh;
	}

	void SubmitDraw(Model const& model, Mesh const& mesh)
	{
		drawList.emplace_back(drawObject{ model, mesh });
	}

	void DrawAxis()
	{


	}

	void DrawAll(std::vector<drawObject> const& drawList)
	{
		auto makePivotVector = [](Model const& model)
		{
			return glm::vec3(
				model.pivotPercent.x * model.scale.x,
				model.pivotPercent.y * model.scale.y,
				model.pivotPercent.z * model.scale.z);
		};

		for (drawObject const& currDraw : drawList)
		{
			//Redo the concept of 'isRendering' to avoid iterating through non rendering objects (remove them from the list is better)
			//if (!currDraw.isRendering)
			//	continue;

			//mvp
			glm::mat4 modelMat = glm::mat4(1.0f);

			modelMat = glm::translate(modelMat, currDraw.model.pos);

			//The vector that does rotation at the pivot specified
			glm::vec3 pivotTransVector = makePivotVector(currDraw.model);

			//modelMat = glm::translate(modelMat, pivotTransVector);

			//modelMat = glm::rotate(modelMat, glm::radians(currDraw.model.rotDegrees.x), worldRight);
			//modelMat = glm::rotate(modelMat, glm::radians(currDraw.model.rotDegrees.y), worldUp);
			//modelMat = glm::rotate(modelMat, glm::radians(currDraw.model.rotDegrees.z), worldForward);

			//modelMat = glm::translate(modelMat, -pivotTransVector);
			modelMat = glm::scale(modelMat, currDraw.model.scale);


			glm::mat4 viewMat = glm::mat4(1.0f);

			//Calculate the view mat

			viewMat = glm::lookAt(currCamera.pos, currCamera.targetPos, currCamera.up);

			glm::mat4 perspectiveMat = glm::mat4(1.0f);
			GLfloat fov = currCamera.FOV;
			GLfloat aspectRatio = static_cast<GLfloat>(applicationPtr->getAspectRatio());
			GLfloat nearPlanePoint = 0.1f;
			GLfloat farPlanePoint = 100.0f;

			perspectiveMat = glm::perspective(glm::radians(currCamera.FOV), aspectRatio, nearPlanePoint, farPlanePoint);

			glm::mat4 mvpMat = glm::mat4(1.0f);
			mvpMat = perspectiveMat * viewMat * modelMat;

			//use default shader for now
			unsigned int programID = assignmentShaders.getShaderID(defaultShader.shaderName);
			glUseProgram(programID);

			GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");

			glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMat[0][0]);
			glBindVertexArray(currDraw.mesh.VAO);

			if (currDraw.mesh.isDrawElements)
				glDrawElements(currDraw.mesh.drawType, 0, GL_UNSIGNED_INT, currDraw.mesh.indices);
			else
				glDrawArrays(currDraw.mesh.drawType, 0, 6);
		}
	}

	void InitMeshes()
	{
		meshMap.emplace(MeshNames::quad, InitQuadMesh(upQuadPositions));
		meshMap.emplace(MeshNames::axis, InitAxis());
	}

	int InitAssignment()
	{
		assignmentShaders.loadShader(defaultShader.shaderName, defaultShader);
		std::cout << "Assignment Shaders Loaded\n";

		InitMeshes();
		ObjectMaker::MakeFloor();

		return 0;
	}

	void UpdateAssignment()
	{
		modelMap.at(ModelNames::floorPlane).scale = floorPlaneScale;
	}

	//Has to be different per thing later
	void RenderDearImguiDefault()
	{
		ImGui::Begin("Settings");

		ImGui::DragFloat3("Camera Position", (float*) &currCamera.pos, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("Camera Target Position", (float*)&currCamera.targetPos, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat("FOV", (float*)&currCamera.FOV, 0.01f, 1.0f, 110.0f);
		ImGui::DragFloat3("Background Color", (float*)&backgroundColor, 0.001f, 0.0f, 1.0f);

		ImGui::End();
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
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		RenderDearImguiDefault();

		SubmitDraw(Model{}, meshMap.at(MeshNames::quad));
		SubmitDraw(Model{}, meshMap.at(MeshNames::axis));
		//SubmitDraw(modelMap.at(ModelNames::floorPlane), meshMap.at(MeshNames::quad));

		DrawAll(drawList);
	}
}


//For the object maker
namespace AssignmentOne
{
	void ObjectMaker::MakeFloor()
	{
		//Make an infinite plane at origin

		Model planeModel;
		planeModel.pos = worldOrigin;
		planeModel.pos.y += floorSpawnPos;

		planeModel.scale = floorPlaneScale;

		planeModel.rotDegrees = glm::vec3(0.0f, 0.0f, 0.0f);

		modelMap.emplace(ModelNames::floorPlane, planeModel);
	}
}


