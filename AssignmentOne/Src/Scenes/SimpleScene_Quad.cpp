#pragma once
#include "SimpleScene_Quad.h"
#include "../shader.hpp"


// Include Dear Imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

namespace Scenes
{
	namespace SceneQuad
	{
		SceneClass::sceneFunctionReturnType Init(SceneClass::dataContainerType& dataCont, float deltaTime)
		{
			std::shared_ptr<QuadData> data = std::static_pointer_cast<QuadData>(dataCont[0]);
			data->programID = LoadShaders("../Common/Shaders/QuadVertexShader.vert", "../Common/Shaders/QuadFragmentShader.frag");

			data->geometryBuffer = 
			{			
						0.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 1.0f, 0.0f,
						0.0f, 1.0f, 0.0f
			};

			data->cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
			data->cameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f); //origin start

			data->fieldOfView = 90.0f;


			data->quadScale = glm::vec3(1.0f, 1.0f, 1.0f);
			data->quadAngles = glm::vec3(0.0f, 0.0f, 0.0f);
			data->quadPos = glm::vec3(0.0f, 0.0f, -3.0f);


			data->backgroundColor = glm::vec3(0.0f, 0.0f, 0.7f);

			glGenVertexArrays(1, &(data->VertexArrayID));
			glBindVertexArray(data->VertexArrayID);

			glGenBuffers(1, &(data->vertexbuffer));

			glBindBuffer(GL_ARRAY_BUFFER, data->vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, data->geometryBuffer.size() * sizeof(GLfloat),
				data->geometryBuffer.data(), GL_STATIC_DRAW);

			return 1;
		}


		//Simple testing if we can have alternative Quad Scenes
		SceneClass::sceneFunctionReturnType InitColor(SceneClass::dataContainerType& dataCont, float deltaTime)
		{
			std::shared_ptr<QuadData> data = std::static_pointer_cast<QuadData>(dataCont[0]);

			data->programID = LoadShaders("../Common/Shaders/QuadVertexShader.vert", "../Common/Shaders/QuadFragmentShader.frag");


			constexpr float quadColorR = 0.0f;
			constexpr float quadColorG = 1.0f;
			constexpr float quadColorB = 0.85f;

			//AOS geometry Buffer
			data->geometryBuffer =
			{			
						//Vertex positions
						0.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						1.0f, 1.0f, 0.0f,
						0.0f, 1.0f, 0.0f,

						//Vertex Colors

						quadColorR, quadColorG, quadColorB,
						quadColorR, quadColorG, quadColorB,
						quadColorR, quadColorG, quadColorB,
						quadColorR, quadColorG, quadColorB,
						quadColorR, quadColorG, quadColorB,
						quadColorR, quadColorG, quadColorB,
			};

			data->cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
			data->cameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f); //origin start

			data->fieldOfView = 90.0f;


			data->quadScale = glm::vec3(1.0f, 1.0f, 1.0f);
			data->quadAngles = glm::vec3(0.0f, 0.0f, 0.0f);
			data->quadPos = glm::vec3(0.0f, 0.0f, -3.0f);


			data->backgroundColor = glm::vec3(0.0f, 0.0f, 0.7f);

			glGenVertexArrays(1, &(data->VertexArrayID));
			glBindVertexArray(data->VertexArrayID);

			glGenBuffers(1, &(data->vertexbuffer));

			glBindBuffer(GL_ARRAY_BUFFER, data->vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, data->geometryBuffer.size() * sizeof(GLfloat),
				data->geometryBuffer.data(), GL_STATIC_DRAW);



			//Now for color

			//const size_t offsetToFirstColor = 15; //in index before converted to bytes
			//const size_t vertexColorStride = 0;
			//const size_t indexOfColor = 1; //in the fragment shader
			//const size_t numberColorPerVertex = 3;

			//glVertexAttribPointer(static_cast<GLuint>(indexOfColor), static_cast<GLint>(numberColorPerVertex), GL_FLOAT, GL_FALSE, vertexColorStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstColor));
			//glEnableVertexAttribArray(indexOfColor);


			return 1;
		}

		SceneClass::sceneFunctionReturnType Render(SceneClass::dataContainerType& dataCont, float deltaTime)
		{
			static float f = 0.0f;
			static int counter = 0;

			std::shared_ptr<QuadData> data = std::static_pointer_cast<QuadData>(dataCont[0]);

			ImGui::Begin("Settings");
			ImGui::DragFloat3("Camera Position", (float*)&data->cameraPos, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3("Camera Target Position", (float*)&data->cameraTargetPos, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat("FOV", &data->fieldOfView, 0.01f, 1.0f, 110.0f);
			ImGui::DragFloat3("Quad-Plane Position", (float*)&data->quadPos, 0.01f, -20.0f, 20.0f);
			ImGui::DragFloat3("Quad-Plane Scale", (float*)&data->quadScale, 0.01f, -20.0f, 20.0f);
			ImGui::DragFloat3("Quad-Plane Rotation (Degrees)", (float*)&data->quadAngles, 0.01f, -360.0f, 360.0f);
			ImGui::DragFloat3("Background Color", (float*)&data->backgroundColor, 0.001f, 0.0f, 1.0f);

			ImGui::End();

			glClearColor(data->backgroundColor.r, data->backgroundColor.g, data->backgroundColor.b, 1.0f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glUseProgram(data->programID);

			//glEnableVertexAttribArray(0);


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

			glEnableVertexAttribArray(indexOfColor);
			glVertexAttribPointer(static_cast<GLuint>(indexOfColor), static_cast<GLint>(numberColorPerVertex), GL_FLOAT, GL_FALSE, vertexColorStride * sizeof(float), (void*)(sizeof(float) * offsetToFirstColor));


			//glBindBuffer(GL_ARRAY_BUFFER, data->vertexbuffer);
			
			
			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);



			// Uniform matrix
			// Uniform transformation (vertex shader)
			GLint vTransformLoc = glGetUniformLocation(data->programID, "vertexTransform");

			// Draw the triangle !
			// T * R * S * Vertex
			glm::mat4 modelMat = glm::mat4(1.0f);
			glm::vec3 scaleVector = glm::vec3(1.0f);
			glm::vec3 centroid = glm::vec3(0.0f, -0.0f, 0.0f);
			glm::vec3 triPos = glm::vec3(0.0f, -0.5f, -1.0f);

			modelMat = glm::translate(modelMat, data->quadPos);

			float pivotPercentFromLeft = 0.5f;

			modelMat = glm::translate(modelMat, glm::vec3(pivotPercentFromLeft * data->quadScale.x, 
				pivotPercentFromLeft * data->quadScale.y, 
				pivotPercentFromLeft * data->quadScale.z));

			modelMat = glm::rotate(modelMat, data->quadAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
			modelMat = glm::rotate(modelMat, data->quadAngles.y, glm::vec3(0.0f, 1.0f, 1.0f));
			modelMat = glm::rotate(modelMat, data->quadAngles.x, glm::vec3(1.0f, 0.0f, 1.0f));
			modelMat = glm::translate(modelMat, glm::vec3(-pivotPercentFromLeft * data->quadScale.x, -pivotPercentFromLeft * data->quadScale.y, -pivotPercentFromLeft * data->quadScale.z));

			modelMat = glm::scale(modelMat, data->quadScale);


			// Prototype perspective projection (to do: refactor this out later)
			glm::mat4 perspectiveMat = glm::mat4(1.0f);
			GLfloat fov = 45.0f;
			GLfloat aspectRatio = static_cast<GLfloat>(data->parentApplication->getAspectRatio());
			GLfloat nearPlanePoint = 0.1f;
			GLfloat farPlanePoint = 100.0f;

			perspectiveMat = glm::perspective(glm::radians(data->fieldOfView), aspectRatio, nearPlanePoint, farPlanePoint);

			//perspectiveMat = glm::mat4(1.0f);
			glm::mat4 viewMat = glm::mat4(1.0f);

			glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
			viewMat = glm::lookAt(data->cameraPos, data->cameraTargetPos, upVector);

			glm::mat4 mvpMat = glm::mat4(1.0f);
			mvpMat = perspectiveMat * viewMat * modelMat;

			// To Do: Calculate and inject the view matrix into here before we pass it to the shader

			glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMat[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(0);

			data->angleOfRotation += 0.01f * (data->parentApplication->getDeltaTime());

			return 0;
		}

		SceneClass CreateQuadScene()
		{
			SceneClass quadScene;
			quadScene.sceneDataContainer.emplace_back(std::make_shared<QuadData>());
			
			SceneClass::sceneFunctionType f = Init;
			quadScene.startupFunctions.push_back(f);

			SceneClass::sceneFunctionType f2 = Render;
			quadScene.runtimeFunctions.push_back(f2);

			return quadScene;
		}

		SceneClass CreateColorQuadScene()
		{
			SceneClass quadScene;
			quadScene.sceneDataContainer.emplace_back(std::make_shared<QuadData>());

			SceneClass::sceneFunctionType f = InitColor;
			quadScene.startupFunctions.push_back(f);

			SceneClass::sceneFunctionType f2 = Render;
			quadScene.runtimeFunctions.push_back(f2);

			return quadScene;
		}
	}
}

