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

	glm::vec3 pointDefaultColor = { 0.0f, 1.0f, 0.0f };


	namespace MeshNames
	{
		//can be used for planes too
		const char quad[] = "quad";
		const char axis[] = "axis";
		const char point[] = "point";
	}

	//To Do: Can allow dynamic model names here too, of course
	namespace ModelNames
	{
		const char floorPlane[] = "floorPlaneModel";
		const char pointModel[] = "pointModel"; //for debugging points
	}

	constexpr bool runDebugTests = true;
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

	// Does the counter-clockwise triangle vertex assignment automatically. 
	// Assumptions: The first vertex passed in is the 'top-left' vertex
	// the coordinates past in are normalized to a 1x1x1 NDC
	// Example: 
	//
	// 1st 
	// |------|
	// |\     |
	// | \    |
	// |  \   |
    // |   \  | 
	// |    \ |
	// |------|
    // 2nd  3rd
	//
	// 4th point is the 3rd
	// 5th point from the diagonal vector from the 2nd
	// 6th point is 1st
	// 
	// relativeUp and relativeRight refers to the unit basis vector that is seen if looking from the quad's normal
	// An example is that if the quad's normal is the positive rightVector, the relativeUp is the upVector but the relativeRight is the negativeForward vector
	// This is useful for calculating a primitive of a sphere's face (as that value can be complicated using theta and phi) or for conviencne (generating 1x1x1 cube)
	std::vector<GLfloat> makeQuadFromVertices(std::vector <glm::vec3> tri, glm::vec3 relativeRight, glm::vec3 relativeUp, float scaleNDC = 2.0f)
	{
		//4th point counting from 1
		tri.emplace_back(glm::vec3{tri[2]});
		tri.emplace_back(glm::vec3{ tri[1] + (relativeRight + relativeUp) * scaleNDC });
		tri.emplace_back(glm::vec3{ tri[0] });

		return verticesFromVectorList(tri);
	}

	std::vector<GLfloat> makeQuadFromPointTopLeft(glm::vec3 topLeft, glm::vec3 relativeRight, glm::vec3 relativeUp, float scaleNDC = 2.0f)
	{
		glm::vec3 bottomLeft = topLeft - (relativeUp) * (scaleNDC);
		glm::vec3 bottomRight = topLeft + (relativeRight - relativeUp) * scaleNDC; //go diagonally
		return makeQuadFromVertices(std::vector<glm::vec3>{topLeft, bottomLeft, bottomRight}, relativeRight, relativeUp, scaleNDC);
	}
	std::vector<GLfloat> makeQuadFromPointBottomLeft(glm::vec3 bottomLeft, glm::vec3 relativeRight, glm::vec3  relativeUp, float scaleNDC = 2.0f)
	{
		return makeQuadFromPointTopLeft(bottomLeft + (relativeUp)*scaleNDC, relativeRight, relativeUp, scaleNDC);
	}

	std::vector<GLfloat> makeQuadFromPointBottomRight(glm::vec3 bottomRight, glm::vec3 relativeRight, glm::vec3  relativeUp, float scaleNDC = 2.0f)
	{
		return makeQuadFromPointTopLeft(bottomRight - (relativeRight - relativeUp) * scaleNDC, relativeRight, relativeUp, scaleNDC);
	}





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

		mesh.arrayCount = meshPositions.size();
		GLchar defaultDrawType = GL_TRIANGLES;
		mesh.drawType = defaultDrawType;

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


	//For seeing the makeQuadFromVertex function in action
	Mesh InitTestAlignedPlanes()
	{
		std::vector<GLfloat> finalBuffer;
		glm::vec3 relativeRight = worldRight;
		glm::vec3 relativeUp = worldUp;
		std::vector<GLfloat> buffer;


		//std::reverse(buffer.begin(), buffer.end());


		finalBuffer.insert(finalBuffer.end(), std::make_move_iterator(buffer.begin()), std::make_move_iterator(buffer.end()));

		glm::vec3 zFrontTL{ -1.0f, 1.0f, 1.0f };

		relativeRight = worldRight;
		relativeUp = worldUp;

		buffer = makeQuadFromPointTopLeft(zFrontTL, relativeRight, relativeUp);
		finalBuffer.insert(finalBuffer.end(), std::make_move_iterator(buffer.begin()), std::make_move_iterator(buffer.end()));


		glm::vec3 yFrontBL{ -1.0f, 1.0f, 1.0f };
		
		relativeRight = worldRight;
		relativeUp = -worldForward;
		buffer.clear();
		buffer = makeQuadFromPointBottomLeft(yFrontBL, relativeRight, relativeUp);

		finalBuffer.insert(finalBuffer.end(), std::make_move_iterator(buffer.begin()), std::make_move_iterator(buffer.end()));



		glm::vec3 xBackBR{ -1.0f, -1.0f, 1.0f };
		relativeRight = worldForward;
		relativeUp = worldUp;

		buffer.clear();
		buffer = makeQuadFromPointBottomRight(xBackBR, relativeRight, relativeUp);

		finalBuffer.insert(finalBuffer.end(), std::make_move_iterator(buffer.begin()), std::make_move_iterator(buffer.end()));

		glm::vec3 color{ 0.6f, 0.0f, 0.34f };
		std::vector<GLfloat> colorBuffer;

		for (size_t i = 0; i < finalBuffer.size() / 3; ++i)
		{
			colorBuffer.push_back(color.r);
			colorBuffer.push_back(color.g);
			colorBuffer.push_back(color.b);
		}

		return initVBOArrays(finalBuffer, colorBuffer);
	}

	//Don't use: it's buggy and unfinished
	Mesh InitPoint()
	{
		std::vector<GLfloat> point
		{
			worldOrigin.x, worldOrigin.y, worldOrigin.z
		};

		std::vector <GLfloat> color
		{
			pointDefaultColor.r, pointDefaultColor.g, pointDefaultColor.b
		};

		Mesh pointMesh = initVBOArrays(point, color);
		pointMesh.drawType = GL_POINT; //just the one I guess
		return pointMesh;
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



		/*	glPointSize(currDraw.model.scale.x);
			glPointColor*/

			//It kind of makes sense to use the scale in the model to set the glPointSize for this draw 
			//if (currDraw.mesh.drawType == GL_POINT)
				

			if (currDraw.mesh.isDrawElements)
				glDrawElements(currDraw.mesh.drawType, 0, GL_UNSIGNED_INT, currDraw.mesh.indices);
			else
				glDrawArrays(currDraw.mesh.drawType, 0, currDraw.mesh.arrayCount);
		}
	}


	void InitMeshes()
	{


		meshMap.emplace(MeshNames::quad, InitQuadMesh(upQuadPositions));
		meshMap.emplace(MeshNames::axis, InitAxis());
		meshMap.emplace("Test", InitTestAlignedPlanes());
		//meshMap.emplace(MeshNames::point, InitPoint());
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

		//SubmitDraw(Model{}, meshMap.at(MeshNames::quad));
		SubmitDraw(Model{}, meshMap.at(MeshNames::axis));

		Model testModel;
		testModel.scale = glm::vec3(0.5f, 0.5f, 0.5f);
		SubmitDraw(testModel, meshMap.at("Test"));
		//Model pointModel;
		//pointModel.scale = glm::vec3(10.0f, 10.0f, 10.0f);
		//SubmitDraw(pointModel, meshMap.at(MeshNames::point));

		//SubmitDraw(modelMap.at(ModelNames::floorPlane), meshMap.at(MeshNames::quad));

		DrawAll(drawList);
		drawList.clear();
	}
}

namespace AssignmentOne
{
	namespace DebugTesting
	{
		//Test the function 'makeQuadFromVertices'
		void TestQuadVertices1()
		{
			//Example used is the z-front of a cube
			std::vector<glm::vec3> tri;
			tri.emplace_back(glm::vec3(-1.0f, 1.0f, 1.0f));
			tri.emplace_back(glm::vec3(-1.0f, -1.0f, 1.0f));
			tri.emplace_back(glm::vec3(1.0f, -1.0f, 1.0f));

			glm::vec3 relativeRight = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 relativeUp = glm::vec3(0.0f, 1.0f, 0.0f);

			std::vector<GLfloat> actual = makeQuadFromVertices(tri, relativeRight, relativeUp);

			std::vector<GLfloat> expected =
			{
				-1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f
			};

			assert(actual == expected && "Test1() Failed");
		}

		void TestQuadVertices2()
		{
			//Example used is the z-front of a cube
			std::vector<glm::vec3> tri;
			tri.emplace_back(glm::vec3(-1.0f, 1.0f, -1.0f));
			tri.emplace_back(glm::vec3(-1.0f, 1.0f, 1.0f));
			tri.emplace_back(glm::vec3(1.0f, 1.0f, 1.0f));

			glm::vec3 relativeRight = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 relativeUp = glm::vec3(0.0f, 0.0f, - 1.0f);

			std::vector<GLfloat> actual = makeQuadFromVertices(tri, relativeRight, relativeUp);

			std::vector<GLfloat> expected =
			{
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f
			};

			assert(actual == expected && "Test2() Failed");
		}

		void TestQuadVertices3()
		{
			glm::vec3 pt(-1.0f, 1.0f, 1.0f);

			glm::vec3 relativeRight = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 relativeUp = glm::vec3(0.0f, 1.0f, 0.0f);

			std::vector<GLfloat> actual = makeQuadFromPointTopLeft(pt, relativeRight, relativeUp);

			std::vector<GLfloat> expected =
			{
				-1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f
			};

			assert(actual == expected && "Test3() Failed");
		}

		void TestQuadVertices4()
		{

			glm::vec3 pt(-1.0f, 1.0f, -1.0f);

			glm::vec3 relativeRight = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 relativeUp = glm::vec3(0.0f, 0.0f, -1.0f);

			std::vector<GLfloat> actual = makeQuadFromPointTopLeft(pt, relativeRight, relativeUp);

			std::vector<GLfloat> expected =
			{
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f
			};

			assert(actual == expected && "Test4() Failed");
		}
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



	int InitAssignment()
	{
		assignmentShaders.loadShader(defaultShader.shaderName, defaultShader);
		std::cout << "Assignment Shaders Loaded\n";

		if (runDebugTests)
		{
			DebugTesting::TestQuadVertices1();
			DebugTesting::TestQuadVertices2();
			DebugTesting::TestQuadVertices3();
			DebugTesting::TestQuadVertices4();
		}


		InitMeshes();


		ObjectMaker::MakeFloor();

		return 0;
	}
}


