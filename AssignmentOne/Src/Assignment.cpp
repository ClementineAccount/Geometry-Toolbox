#include "Assignment.h"
#include "Transform.h"
#include "CollisionHelper.h"
#include "Camera.h"
#include "BoundingVolume.h"

#include "Object.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <map>

// Include Dear Imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <glm/gtx/rotate_vector.hpp>

//#define ASSIGNMENT_ONE

//Settings
namespace Assignment
{
	const std::string shaderFolderPath = "Shaders/";

	static std::string vertexShaderPath = shaderFolderPath + "VertexShader.vert";
	static std::string fragShaderPath = shaderFolderPath + "FragShader.frag";

	static shaderFilePath defaultShader{ vertexShaderPath, fragShaderPath, "defaultShader" };
	static shaderFilePath colorShader{ shaderFolderPath + "UniformColor.vert", shaderFolderPath + "UniformColor.frag", "colorShader" };

	static bool wireFrameMode = false;

	static glm::vec3 niceOrangeColor{ 1.0f, 0.341f, 0.2f };
	static glm::vec3 coolPurpleColor{ 0.565f, 0.046f, 0.243f };
	static glm::vec3 maroonColor{ 0.78f, 0.0f, 0.224f };
	static glm::vec3 coolOrange{ 1.0f, 0.765f, 0.0f };
	static glm::vec3 basicBlue{ 0.0f, 0.0f, 1.0f };
	static glm::vec3 skyBlue{ 0.149f,0.902f,1.0f };
	static glm::vec3 greenscreenGreen{ 0.0f, 1.0f, 0.f };
	static glm::vec3 blackish{ 0.1f, 0.05f, 0.0f };

	static glm::vec3 defaultRingColor = greenscreenGreen;
	static glm::vec3 defaultSphereColor = greenscreenGreen;
	static glm::vec3 defaultQuadColor = coolOrange;
	static glm::vec3 defaultCubeColor = basicBlue;

	static glm::vec3 collidedBackgroundColor = basicBlue;
	static glm::vec3 prevBackGround;
	static glm::vec3 neutralBackgroundColor = blackish;

	static glm::vec3 backgroundColor = blackish;

	static bool changeBackGroundOnCollision = true;

	static bool wireFramePictureInPicture = false;
	static bool showAxis = true;

	static bool enableDepthTest = true;
	static bool collisionDetected = false;

	constexpr glm::vec3 planeScale = { 1.0f, 1.0f, 1.0f };
	glm::vec3 floorPlaneScale = { 1000.0f, 1.0f, 1000.0f };

	constexpr float floorSpawnPos = -0.01f;

	//Cannot store as glm::vec3 because 'Right' might not mean x-axis
	constexpr float axisScaleRight = 100.0f;
	constexpr float axisScaleUp = 100.0f;
	constexpr float axisScaleForward = 100.0f;

	glm::vec3 axisColorRight{ 1.0f, 0.0f, 0.0f };
	glm::vec3 axisColorUp{ 0.0f, 1.0f, 0.0f };
	glm::vec3 axisColorForward{ 0.0f, 0.0f, 1.0f };

	glm::vec3 pointDefaultColor = { 0.0f, 1.0f, 0.0f };


	float topDownCameraHeight = 6.0f;
	//glm::vec3 topDownCameraPos = { 0.0f, 3.0f, 0.0f };

	//from the bottomleft
	glm::vec2 positionTopRightViewportPercent = { 0.55f, 0.55f };

	//should be based off aspect ratio
	glm::vec2 scaleTopRightViewportPercent = { 0.25f, 0.25f };



	//To Do: Can allow dynamic model names here too, of course
	namespace ModelNames
	{
		const char defaultModel[] = "default";
		const char floorPlane[] = "floorPlaneModel";
		const char pointModel[] = "pointModel"; //for debugging posBuffer
	}

	constexpr bool runDebugTests = true;
}


//Misc (Mesh, Drawing, ...)
namespace Assignment
{
	std::map<std::string, AssignmentScene> sceneMap;

	std::unordered_map<std::string, Transform> modelMap;

	MeshBuffers quadMesh;
	ShaderContainer assignmentShaders;

	//Default camera
	Camera defaultLookAtCamera;
	Camera topDownCamera = defaultLookAtCamera;
	Camera currCamera = defaultLookAtCamera;

	std::string currentSceneName;


	std::vector<GLfloat> quadNormalUpPos =
	{
		-1.0f, 0.0f,  1.0f,  // 0
		 1.0f, 0.0f,  1.0f,  // 1
		 1.0f, 0.0f, -1.0f,  // 2
		 1.0f, 0.0f, -1.0f,  // 3
		-1.0f, 0.0f,  1.0f, // 4
		-1.0f, 0.0f, -1.0f  // 5
	};

	std::vector<GLfloat> quadNormalRightPos =
	{
		 0.0f, -1.0f, -1.0f,  // 0
		 0.0f, 1.0f,  -1.0f,  // 1
		 0.0f, -1.0f, 1.0f,  // 2
		 0.0f, -1.0f, 1.0f,  // 3
		 0.0f, 1.0f,  1.0f, // 4
		 0.0f, 1.0f, -1.0f  // 5
	};

	//Camera firstPersonCamera;


	ViewportModel topRightViewport;
	ViewportModel topRightViewportBorder; //for a border effect

	GeometryToolbox::GLApplication* applicationPtr;

	void setApplicationPtr(GeometryToolbox::GLApplication& appPtr)
	{
		applicationPtr = &appPtr;
	}

	static std::vector<drawCall> drawList;




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



	
	//Using GL lines
	MeshBuffers InitAxis(glm::vec3 axisRight = worldRight, glm::vec3 axisUp = worldUp, glm::vec3 axisForward = worldForward)
	{
		glm::vec3 rightAxis = worldOrigin + (axisRight * axisScaleRight);
		glm::vec3 upAxis = worldOrigin + (axisUp * axisScaleUp);
		glm::vec3 forwardAxis = worldOrigin + (axisForward * axisScaleForward);

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

		MeshBuffers axisMesh = initVBO(axisPoints, axisColor);
		axisMesh.drawType = GL_LINES;
		return axisMesh;
	}


	//Line that is affected by MVP projection (so it exists in the world). The default orientation is pointing to the Right vector
	MeshBuffers InitWorldLine(glm::vec3 basisVector = unitLineBasisVector, float lineScale = unitLineScale)
	{
		glm::vec3 vectorDir = worldOrigin + (basisVector * lineScale);
		std::vector<GLfloat> linePoints
		{
			worldOrigin.x, worldOrigin.y, worldOrigin.z,
			vectorDir.x, vectorDir.y, vectorDir.z,
		};

		//Doesn't matter because we will eventually use another fragment shader for it
		std::vector<GLfloat> lineColor
		{
			axisColorRight.r, axisColorRight.g, axisColorRight.b,
			axisColorRight.r, axisColorRight.g, axisColorRight.b,
		};

		MeshBuffers axisMesh = initVBO(linePoints, lineColor);
		axisMesh.drawType = GL_LINES;
		return axisMesh;
	}

	//Unit 1x1x1 Cube for Transform transform
	//MeshBuffers InitCubeMesh(float cubeScale = 0.5f)
	//{
	//	return MeshBuffers mesh;
	//}

	//For seeing the makeQuadFromVertex function in action
	MeshBuffers InitTestAlignedPlanes()
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

		return initVBO(finalBuffer, colorBuffer);
	}

	//Don't use: it's buggy and unfinished
	MeshBuffers InitPoint()
	{
		std::vector<GLfloat> point
		{
			worldOrigin.x, worldOrigin.y, worldOrigin.z
		};

		std::vector <GLfloat> color
		{
			pointDefaultColor.r, pointDefaultColor.g, pointDefaultColor.b
		};

		MeshBuffers pointMesh = initVBO(point, color);
		pointMesh.drawType = GL_POINT; //just the one I guess
		return pointMesh;
	}

	//To Do: Move all these into a mesh construction class? 
	namespace MeshConstructor
	{
		namespace Sphere
		{
			//Note: Stacks and Slices count from 0
			static float getAngleSegmentDegrees(unsigned int currSegment, unsigned int totalSegments, float startingOffsetDegrees, float angleMax)
			{
				float anglePerStack = angleMax / totalSegments;
				return (startingOffsetDegrees + anglePerStack * (currSegment)); 
			}

			//Start from the bottom of the sphere means we must rotate ccw by 270 first by default (so we can start from lowest point)
			static float getThetaDegrees(unsigned int currStack, unsigned int totalStacks, float startingOffsetDegrees = 270.0f, float angleMax = 180.0f)
			{
				return getAngleSegmentDegrees(currStack, totalStacks, startingOffsetDegrees, angleMax);

			}

			static float getPhiDegrees(unsigned int currSlice, unsigned int totalSlices, float startingOffsetDegrees = 0.0f, float angleMax = 360.0f)
			{
				return getAngleSegmentDegrees(currSlice, totalSlices, startingOffsetDegrees, angleMax);
			}
		}
	}

	//adapted from: http://www.songho.ca/opengl/gl_sphere.html (tried it myself but am running out of time)
	//The site's guide uses pi and radians directly, while I had attempted to do it using degrees (if u study the test cases)
	//I might rewrite/refactor this to use my own functions if I have time, but I have been spending too long on this sphere thing
	MeshBuffers initSphereMesh(glm::vec3 sphereColor = defaultSphereColor, unsigned int numSlices = 36, unsigned int numStacks = 18, float sphereRadius = 1.0f)
	{
		

		float sliceStep = 2 * glm::pi<float>() / numSlices;
		float stackStep = glm::pi<float>() / numStacks;
		float sliceAngle, stackAngle;

		float x, y, z, stack_xy;
		std::vector<GLfloat> positions;

		for (size_t currStack = 0; currStack <= numStacks; ++currStack)
		{
			//starting from the bottom stack (so we rotate ccw by 270 and then add onto that)
			stackAngle = glm::pi<float>() / 2 - currStack * stackStep;

			stack_xy = sphereRadius * cosf(stackAngle);
			z = sphereRadius * sinf((stackAngle));

			for (size_t currSlice = 0; currSlice <= numSlices; ++currSlice)
			{
				sliceAngle = currSlice * sliceStep;

				x = stack_xy * cosf(sliceAngle);
				y = stack_xy * sinf(sliceAngle);

				positions.push_back(x);
				positions.push_back(y);
				positions.push_back(z);
			}
		}

		//http://www.songho.ca/opengl/gl_sphere.html

		std::vector<GLfloat> colorBuffer;

		for (size_t i = 0; i < positions.size() / 3; ++i)
		{
			colorBuffer.push_back(sphereColor.r);
			colorBuffer.push_back(sphereColor.g);
			colorBuffer.push_back(sphereColor.b);
		}
		MeshBuffers sphereMesh = initVBO(positions, colorBuffer);


		//Genererate the EBO
		std::vector<int> indices;
		int k1, k2;
		for (int i = 0; i < numStacks; ++i)
		{
			k1 = i * (numSlices + 1);     // beginning of current stack
			k2 = k1 + numSlices + 1;      // beginning of next stack

			for (int j = 0; j < numSlices; ++j, ++k1, ++k2)
			{
				//Same counter-clockwise idea
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (numStacks - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		glGenBuffers(1, &sphereMesh.EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereMesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

		sphereMesh.elementCount = indices.size();
		sphereMesh.isDrawElements = true;
		sphereMesh.drawType = GL_TRIANGLES; //So we can do texture stuff with this later
		return sphereMesh;
	}
	
	MeshBuffers initCubeMesh(glm::vec3 cubeColor = defaultCubeColor, float cubeScale = 0.5f)
	{
		glm::vec3 relativeUp;
		glm::vec3 relativeRight;

		std::vector<GLfloat> buffer;

		std::vector<GLfloat> posBuffer;

		auto makeFace = [&posBuffer](glm::vec3 bottomLeft, glm::vec3 relativeUp, glm::vec3 relativeRight)
		{
			std::vector<GLfloat> buffer;
			buffer = makeQuadFromPointBottomLeft(bottomLeft, relativeRight, relativeUp);
			posBuffer.insert(posBuffer.end(), buffer.begin(), buffer.end());
		};

		//z face (front)
		makeFace(glm::vec3(-1.0f, -1.0f, 1.0f), worldUp, worldRight);

		//z face (back)
		makeFace(glm::vec3(1.0f, -1.0f, 1.0f), worldUp, -worldRight);

		//y face (front)
		makeFace(glm::vec3(-1.0f, 1.0f, 1.0f), -worldForward, worldRight);

		//y face (bak)
		makeFace(glm::vec3(1.0f, -1.0f, 1.0f), -worldForward, -worldRight);

		//x face (front)
		makeFace(glm::vec3(1.0f, -1.0f, 1.0f), worldUp, -worldForward);

		//x face (back)
		makeFace(glm::vec3(-1.0f, -1.0f, -1.0f), worldUp, worldForward);


		for (GLfloat& i : posBuffer)
			i *= cubeScale;


		std::vector<GLfloat> colorBuffer;

		for (size_t i = 0; i < posBuffer.size() / 3; ++i)
		{
			colorBuffer.push_back(defaultCubeColor.r);
			colorBuffer.push_back(defaultCubeColor.g);
			colorBuffer.push_back(defaultCubeColor.b);
		}

		MeshBuffers cubeMesh = initVBO(posBuffer, colorBuffer);
		cubeMesh.drawType = GL_TRIANGLES;
		return cubeMesh;
	}

	MeshBuffers InitQuadMesh(std::vector<GLfloat>& quadPositions, float quadScale)
	{
		//Allows me to set quad positions normalized to 1 unit while applying the 0.5 scale afterwards
		for (GLfloat& i : quadPositions)
			i *= quadScale;

		float quadColorR = defaultQuadColor.r;
		float quadColorG = defaultQuadColor.g;
		float quadColorB = defaultQuadColor.b;

		std::vector<GLfloat> quadColors =
		{
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
			quadColorR, quadColorG, quadColorB,
		};

		MeshBuffers quadMesh = initVBO(quadPositions, quadColors);
		return quadMesh;
	}

	void SubmitDraw(std::string const modelName, std::string const meshName, std::string const shaderName = defaultShader.shaderName, unsigned int drawOrder = 0)
	{
		drawList.push_back(drawCall{ modelMap.at(modelName), meshMap.at(meshName), assignmentShaders.getShaderID(shaderName) });


	}

	void SubmitDraw(Transform model, std::string const meshName, std::string const shaderName = defaultShader.shaderName, unsigned int drawOrder = 0)
	{
		drawList.push_back(drawCall{ model, meshMap.at(meshName), assignmentShaders.getShaderID(shaderName)});

	}

	void SubmitDraw(Transform model, MeshBuffers const& meshBuffer, std::string const shaderName = defaultShader.shaderName, unsigned int drawOrder = 0)
	{
		drawList.push_back(drawCall{ model, meshBuffer, assignmentShaders.getShaderID(shaderName) });
	}

	//Fills the entire screen with a quad (example of usage to create a border for Top Left picture-in-view)
	void FillScreen(glm::vec3 colorFill = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		MeshBuffers quadMesh = meshMap.at(MeshNames::quadNormalForward);

		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::scale(modelMat, glm::vec3(1000.0f, 1000.0f, 1000.0f));

		unsigned int programID = assignmentShaders.getShaderID(colorShader.shaderName);
		glUseProgram(programID);

		GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");
		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);

		GLint colorLoc = glGetUniformLocation(programID, "setColor");
		glUniform3fv(colorLoc, 1, (float*)&colorFill);


		glBindVertexArray(quadMesh.VAO);
		glDrawArrays(quadMesh.drawType, 0, quadMesh.arrayCount);
	}


	void DrawAll(std::vector<drawCall>& drawList, Camera const& drawCamera)
	{
		auto makePivotVector = [](Transform const& model)
		{
			return glm::vec3(
				model.pivotPercent.x * model.scale.x,
				model.pivotPercent.y * model.scale.y,
				model.pivotPercent.z * model.scale.z);
		};

		//auto sortDraw = [](drawCall const& lhs, drawCall const& rhs) {
		//	return lhs.drawOrder < rhs.drawOrder;
		//};

		//std::sort(drawList.begin(), drawList.end(), [](drawCall lhs, drawCall rhs) {
		//	return lhs.drawOrder < rhs.drawOrder; });
		

		for (drawCall const& currDraw : drawList)
		{
			//Redo the concept of 'isRendering' to avoid iterating through non rendering objects (remove them from the list is better)
			//if (!currDraw.isRendering)
			//	continue;

			Transform const& currModel = currDraw.model;

			//mvp
			glm::mat4 modelMat = glm::mat4(1.0f);

			modelMat = glm::translate(modelMat, currModel.pos);

			//The vector that does rotation at the pivot specified
			glm::vec3 pivotTransVector = makePivotVector(currModel);

			//modelMat = glm::translate(modelMat, pivotTransVector);

			modelMat = glm::rotate(modelMat, glm::radians(currModel.rotDegrees.x), worldRight);
			modelMat = glm::rotate(modelMat, glm::radians(currModel.rotDegrees.y), worldUp);
			modelMat = glm::rotate(modelMat, glm::radians(currModel.rotDegrees.z), worldForward);

			//modelMat = glm::translate(modelMat, -pivotTransVector);
			modelMat = glm::scale(modelMat, currModel.scale);


			glm::mat4 viewMat = glm::mat4(1.0f);

			//Calculate the view mat

			viewMat = glm::lookAt(drawCamera.pos, drawCamera.targetPos, drawCamera.up);

			glm::mat4 perspectiveMat = glm::mat4(1.0f);
			GLfloat fov = drawCamera.FOV;
			GLfloat aspectRatio = static_cast<GLfloat>(applicationPtr->getAspectRatio());
			GLfloat nearPlanePoint = 0.1f;
			GLfloat farPlanePoint = 100.0f;

			perspectiveMat = glm::perspective(glm::radians(drawCamera.FOV), aspectRatio, nearPlanePoint, farPlanePoint);

			glm::mat4 mvpMat = glm::mat4(1.0f);
			mvpMat = perspectiveMat * viewMat * modelMat;

			//use default shader for now
			unsigned int programID = currDraw.shaderID;
			glUseProgram(currDraw.shaderID);

			MeshBuffers const& currMesh = currDraw.mesh;

			GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");

			glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMat[0][0]);

			if (currDraw.shaderID == assignmentShaders.getShaderID(colorShader.shaderName))
			{
				GLint colorLoc = glGetUniformLocation(programID, "setColor");
				glUniform3fv(colorLoc, 1, (float*)&currDraw.model.color);
			}

			glBindVertexArray(currMesh.VAO);

			if (currMesh.isDrawElements)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currMesh.EBO);
				glDrawElements(currMesh.drawType, currMesh.elementCount, GL_UNSIGNED_INT, 0);
			}

			else
				glDrawArrays(currMesh.drawType, 0, currMesh.arrayCount);
		}
	}

	void InitMeshes()
	{
		glm::vec3 pt(-1.0f, 0.0f, 1.0f);

		glm::vec3 relativeRight = worldRight;
		glm::vec3 relativeUp = -worldForward;

		std::vector<GLfloat> quadPlanepos = makeQuadFromPointBottomLeft(pt, relativeRight, relativeUp);

		meshMap.emplace(MeshNames::quad, InitQuadMesh(quadPlanepos));

		pt = glm::vec3(-1.0f, -1.0f, 0.0f);

		relativeRight = worldRight;
		relativeUp = worldUp;

		quadPlanepos = makeQuadFromPointBottomLeft(pt, relativeRight, relativeUp);

		meshMap.emplace(MeshNames::quadNormalForward, InitQuadMesh(quadPlanepos));
		meshMap.emplace(MeshNames::quadNormalUp, InitQuadMesh(quadNormalUpPos));
		meshMap.emplace(MeshNames::quadNormalRight, InitQuadMesh(quadNormalRightPos));

		meshMap.emplace(MeshNames::axis, InitAxis());
		meshMap.emplace(MeshNames::axisInverted, InitAxis(-worldRight, -worldUp, -worldForward));
		meshMap.emplace(MeshNames::cube, initCubeMesh());

		meshMap.emplace(MeshNames::rayUp, InitWorldLine(worldUp));
		meshMap.emplace(MeshNames::rayRight, InitWorldLine(worldRight));
		meshMap.emplace(MeshNames::rayForward, InitWorldLine(worldForward));

		meshMap.emplace(MeshNames::worldLine, InitWorldLine());
		meshMap.emplace(MeshNames::sphere, initSphereMesh());

		meshMap.emplace("Test", InitTestAlignedPlanes());
		//meshMap.emplace(MeshNames::point, InitPoint());
	}

	//Moves the camera while preserving the targetVector (and hence the target will be adjusted
	void MoveCameraAligned(Camera& moveCamera, glm::vec3 trans)
	{
		//Don't normalize so as to preserve Near/Far plane relationship for frustrum
		glm::vec3 targetVector = (moveCamera.targetPos - moveCamera.pos); 
		
		moveCamera.pos += trans;
		moveCamera.targetPos = moveCamera.pos + targetVector;
	}

	void SetScene(std::string setScene)
	{
		drawList.clear();

		//That way there is a default background for every scene
		backgroundColor = neutralBackgroundColor;

		sceneMap.at(setScene).initScene();
		currentSceneName = setScene;
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

	void InitPictureInPicture()
	{
		topDownCamera.targetPos = currCamera.targetPos;
		topDownCamera.pos = topDownCamera.targetPos;
		topDownCamera.pos.y = topDownCameraHeight;

		//Hardcoded testing of ideas in pixels

		//in pixels

		//Add this twice to each edge (think of a picture frame)
		float borderSideLength = 2.0f;
		float borderSideHeight = 2.0f;

		topRightViewport.viewportScale = glm::vec2(200.0f, 200.0f);
		topRightViewport.bottomLeft.x = (applicationPtr->getWindowWidth() - topRightViewport.viewportScale.x - topRightViewport.viewportScale.x * 0.1f);
		topRightViewport.bottomLeft.y = (applicationPtr->getWindowHeight() - topRightViewport.viewportScale.y - topRightViewport.viewportScale.y * 0.1f);


		topRightViewportBorder.bottomLeft = topRightViewport.bottomLeft;
		topRightViewportBorder.bottomLeft.x -= borderSideLength;
		topRightViewportBorder.bottomLeft.y -= borderSideHeight;


		topRightViewportBorder.viewportScale = glm::vec2(topRightViewport.viewportScale.x + borderSideLength * 2.0f, topRightViewport.viewportScale.y + borderSideHeight * 2.0f);
	}
	
	void RenderPictureinPicture()
	{
		//topDownCamera.targetPos = currCamera.targetPos;
		topDownCamera.pos = topDownCamera.targetPos;
		topDownCamera.pos.y = topDownCameraHeight;
		topDownCamera.right = worldRight;
		topDownCamera.up = -worldForward;

		//Treat the viewports like transformations.

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		glViewport(topRightViewportBorder.bottomLeft.x, topRightViewportBorder.bottomLeft.y, topRightViewportBorder.viewportScale.x, topRightViewportBorder.viewportScale.y);
		FillScreen();


		glViewport(topRightViewport.bottomLeft.x, topRightViewport.bottomLeft.y, topRightViewport.viewportScale.x, topRightViewport.viewportScale.y);
		
		FillScreen(backgroundColor);


		if (wireFramePictureInPicture)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glEnable(GL_DEPTH_TEST);
		DrawAll(drawList, topDownCamera);
	}

	void RenderAxis()
	{
		if (showAxis)
		{
			SubmitDraw(ModelNames::defaultModel, MeshNames::axisInverted);
			SubmitDraw(ModelNames::defaultModel, MeshNames::axis);
		}
	}


	void RenderAssignmentTesting()
	{

		Transform testModel;
		testModel.scale = glm::vec3(0.5f, 0.5f, 0.5f);

		Transform sphereModel;
		sphereModel.scale = glm::vec3(10.0f, 10.0f, 10.0f);

		//SubmitDraw(Transform{}, meshMap.at(MeshNames::sphere));

		RenderAxis();
		//SubmitDraw(ModelNames::defaultModel, MeshNames::axisInverted);
		//SubmitDraw(ModelNames::defaultModel, MeshNames::axis);


		static Transform worldLineTestMode;
		worldLineTestMode.scale = glm::vec3(10.0f, 10.0f, 10.0f);
		worldLineTestMode.pos = glm::vec3(0.0f, 0.5f, 0.0f);
		worldLineTestMode.rotDegrees.y += applicationPtr->getDeltaTime() * 100.0f;

		SubmitDraw(worldLineTestMode, MeshNames::worldLine);

		static Transform testCube;
		testCube.scale = glm::vec3(1.0f, 1.0f, 1.0f);
		testCube.rotDegrees.y += applicationPtr->getDeltaTime() * 10.0f;


		DrawAll(drawList, currCamera);

	}

	
	//The three models for the triangle to match the rotation
	void RotateTriangleLines(Triangle& triangle)
	{
		//Also ensures the scale is correctly
		//Rotations are based off the right axis
		triangle.rayA.pos = triangle.pt1;
		triangle.rayB.pos = triangle.pt1;
		triangle.rayC.pos = triangle.pt2;

		//angle from right vector to the vector (to do: cache this when you rotate)
		glm::vec3 edgeA = (triangle.pt0 - triangle.pt1);

		//Note this is in radians --> since we are projecting onto the x-axis, we can skip dot product calculation
		float angleBetween = acosf(edgeA.x / glm::length(edgeA));

		triangle.rayA.rotDegrees = glm::vec3(0.0f, 0.0f, glm::degrees(angleBetween));
		triangle.rayA.scale.x = glm::length(edgeA);

		glm::vec3 edgeB = (triangle.pt2 - triangle.pt1);

		angleBetween = acosf(edgeB.x / glm::length(edgeB));

		triangle.rayB.rotDegrees = glm::vec3(0.0f, 0.0f, glm::degrees(angleBetween));
		triangle.rayB.scale.x = glm::length(edgeB);

		glm::vec3 edgeC = (triangle.pt0 - triangle.pt2);

		angleBetween = acosf(edgeC.x / glm::length(edgeC));

		triangle.rayC.rotDegrees = glm::vec3(0.0f, 0.0f, glm::degrees(angleBetween));
		triangle.rayC.scale.x = glm::length(edgeC);
	}

	void UpdateTriangle(Triangle& triangle)
	{
		RotateTriangleLines(triangle);
	}


	//Limitation of gimbal lock rotation. Applie rotation by the same order as in the mvp for now
	void RotatePlane(Plane& plane, float rotateRightDegrees, float rotateUpDegrees, float rotateForwardDegrees)
	{
		glm::vec3 normalVector = worldRight;

		normalVector = glm::rotateX(normalVector, glm::radians(rotateRightDegrees));
		normalVector = glm::rotateY(normalVector, glm::radians(rotateUpDegrees));
		normalVector = glm::rotateZ(normalVector, glm::radians(rotateForwardDegrees));

		plane.outwardNormal = normalVector;
		//y-axis visual representation a little off and I don't know why 
		plane.model.rotDegrees = glm::vec3(rotateRightDegrees, rotateUpDegrees, rotateForwardDegrees);
		plane.normalModel.rotDegrees = glm::vec3(rotateRightDegrees, rotateUpDegrees, rotateForwardDegrees);

	}

	void UpdatePlane(Plane& plane)
	{
		plane.model.pos = plane.pointOnPlane;
		plane.normalModel.pos = plane.pointOnPlane;

		//Reset the rotation normal first before reapplying the rotation from this point
		plane.outwardNormal = worldRight;
		RotatePlane(plane, plane.rotation.x, plane.rotation.y, plane.rotation.z);
	}

	void RotateRay(Ray& ray, float rightDegrees, float upDegrees, float forwardDegrees)
	{
		//Reset to the right (ensure the ray model matches)
		ray.meshID = MeshNames::rayRight; 
		glm::vec3 newDirection = worldRight;

		newDirection = glm::rotateX(newDirection, glm::radians(rightDegrees));
		newDirection = glm::rotateY(newDirection, glm::radians(upDegrees));
		newDirection = glm::rotateZ(newDirection, glm::radians(forwardDegrees));

		ray.direction = newDirection;

		ray.model.rotDegrees = glm::vec3(rightDegrees, upDegrees, forwardDegrees);
	}

	void UpdateRay(Ray& ray)
	{
		//Rotation in mvp for ray should be from the leftmost point of the VBO
		ray.model.pivotPercent = glm::vec3(0.0f, 0.0f, 0.0f);
		ray.model.pos = ray.startPoint;

		//Based off right vector
		ray.model.scale.x = ray.length;

		RotateRay(ray, ray.model.rotDegrees.x, ray.model.rotDegrees.y, ray.model.rotDegrees.z);
	}

	void UpdatePhysics(glm::vec3& pos, Kinematics const& kinematics)
	{
		if (glm::length(kinematics.normVector) - 0.0f <= std::numeric_limits<float>::epsilon())
			return;

		//Currently assumes constant acceleration (also instanaous velocity anyways)

		pos += static_cast<GLfloat>(applicationPtr->getDeltaTime()) * (kinematics.speed * glm::normalize(kinematics.normVector));
	}

	void LoadScene(std::vector<Object>& objectVectorRef, std::string const& scenePath)
	{
		std::string modelFolderPath = "Models/";

		std::ifstream sceneFile;
		sceneFile.open(scenePath);

		//Should do proper error handling if reusing outside Assignment 2 
		if (!sceneFile.is_open())
		{
			assert("File not found!");
		}
		else 
		{
			std::string buffer;

			while (!sceneFile.eof())
			{
				Object obj;
				sceneFile >> buffer;
				obj.objectMesh.loadOBJ(modelFolderPath + buffer);

				auto vec3Buffer = [&](glm::vec3& vec3Ref)
				{
					sceneFile >> buffer;
					vec3Ref.x = std::stof(buffer);

					sceneFile >> buffer;
					vec3Ref.y = std::stof(buffer);

					sceneFile >> buffer;
					vec3Ref.z = std::stof(buffer);
				};

				vec3Buffer(obj.transform.pos);
				vec3Buffer(obj.transform.scale);
				vec3Buffer(obj.transform.rotDegrees);
				vec3Buffer(obj.transform.color);

				objectVectorRef.push_back(obj);
			}

		}
		sceneFile.close();

	}
}

//Dear Imgui UI
namespace Assignment
{

	//Has to be different per thing later
	void RenderDearImguiDefault()
	{
		ImGui::Begin("Settings");

		ImGui::Text(collisionDetected ? "Collision" : "No Collision");

		ImGui::DragFloat3("Camera Position", (float*)&currCamera.pos, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("Camera Target Position", (float*)&currCamera.targetPos, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat("FOV", (float*)&currCamera.FOV, 0.01f, 1.0f, 110.0f);
		ImGui::DragFloat3("Background Color", (float*)&backgroundColor, 0.001f, 0.0f, 1.0f);



		ImGui::Checkbox("Background Color Change on Collision", &changeBackGroundOnCollision);
		if (!changeBackGroundOnCollision)
			collidedBackgroundColor = backgroundColor;
		else
			collidedBackgroundColor = basicBlue;


		ImGui::Checkbox("Wireframe Mode", &wireFrameMode);
		ImGui::Checkbox("Wireframe Picture in Picture", &wireFramePictureInPicture);
		ImGui::Checkbox("Show Axis", &showAxis);
		ImGui::Checkbox("Enable Depth Testing", &enableDepthTest);

		ImGui::End();

		ImGui::Begin("Simple Camera Controls");

		static float cameraMoveSpeed = 1.2f;
		ImGui::TextWrapped("Move along the world axis");
		ImGui::BulletText("Blue: Forward");
		ImGui::BulletText("Red: Right");
		ImGui::BulletText("Yellow: Up");

		ImGui::Columns(2, nullptr, false);

		ImGui::Button("Zoom In");
		if (ImGui::IsItemActive())
		{
			MoveCameraAligned(currCamera, (currCamera.targetPos - currCamera.pos) * cameraMoveSpeed * static_cast<float>(applicationPtr->getDeltaTime()));
		}

		ImGui::Button("Zoom Out");
		if (ImGui::IsItemActive())
		{
			MoveCameraAligned(currCamera, -(currCamera.targetPos - currCamera.pos) * cameraMoveSpeed * static_cast<float>(applicationPtr->getDeltaTime()));
		}

		auto moveBothCam = [&](glm::vec3 moveDirection) {
			MoveCameraAligned(topDownCamera, moveDirection * cameraMoveSpeed * static_cast<float>(applicationPtr->getDeltaTime()));
			MoveCameraAligned(currCamera, moveDirection * cameraMoveSpeed * static_cast<float>(applicationPtr->getDeltaTime()));
		};



		ImGui::Button("Right");
		if (ImGui::IsItemActive())
		{
			moveBothCam(worldRight);
		}

		ImGui::Button("Left");
		if (ImGui::IsItemActive())
		{
			moveBothCam(-worldRight);
		}
		ImGui::Text("Picture in Picture Zoom");
		ImGui::DragFloat("Zoom Level", (float*)&topDownCameraHeight, 0.05f, 0.0f, 40.0f);

		ImGui::NextColumn();

		ImGui::Button("Forward");
		if (ImGui::IsItemActive())
		{
			moveBothCam(worldForward);
		}

		ImGui::Button("Back");
		if (ImGui::IsItemActive())
		{
			moveBothCam(-worldForward);
		}

		ImGui::Button("Up");
		if (ImGui::IsItemActive())
		{
			MoveCameraAligned(currCamera, worldUp * cameraMoveSpeed * static_cast<float>(applicationPtr->getDeltaTime()));
		}

		ImGui::Button("Down");
		if (ImGui::IsItemActive())
		{
			MoveCameraAligned(currCamera, -worldUp * cameraMoveSpeed * static_cast<float>(applicationPtr->getDeltaTime()));
		}



		ImGui::End();

		ImGui::Begin("Scenes");
		ImGui::Text(("Current Scene: " + currentSceneName).c_str());
		std::map<std::string, AssignmentScene>::iterator it;
		for (it = sceneMap.begin(); it != sceneMap.end(); it++)
		{
			if (ImGui::Button(it->first.c_str()))
			{
				SetScene(it->first.c_str());
			}
		}
		ImGui::End();
	}

	void RenderSphereUI(SphereCollider& sphere, std::string const& sphereName)
	{
		if (ImGui::CollapsingHeader(("Settings for " + sphereName).c_str()))
		{
			ImGui::DragFloat3(("centerPos (" + sphereName + ")").c_str(), (float*)&sphere.centerPos, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat(("radius (" + sphereName + ")").c_str(), (float*)&sphere.radius, 0.01f, -10.0f, 10.0f);
		}
	}

	void RenderAABBUI(AABB& aabb, std::string const& aabbName)
	{
		if (ImGui::CollapsingHeader(("Settings for " + aabbName).c_str()))
		{
			ImGui::DragFloat3(("centerPos (" + aabbName + ")").c_str(), (float*)&aabb.centerPos, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat3(("scale (" + aabbName + ")").c_str(), (float*)&aabb.scale, 0.01f, -10.0f, 10.0f);
		}
	}

	void RenderPlaneUI(Plane& plane, std::string const& planeName)
	{
		if (ImGui::CollapsingHeader(("Settings for " + planeName).c_str()))
		{
			ImGui::DragFloat3(("point (" + planeName + ")").c_str(), (float*)&plane.pointOnPlane, 0.01f, -10.0f, 10.0f);
			static std::string normalText = planeName + "'s normal";
			ImGui::Text(normalText.c_str());
			ImGui::Text("%f, %f, %f", plane.outwardNormal.x, plane.outwardNormal.y, plane.outwardNormal.z);
			ImGui::DragFloat3(("rotation (" + planeName + ")").c_str(), (float*)&plane.rotation, 1.0f, 360.0f, 30.0f);
			ImGui::Checkbox(("show normal (" + planeName + ")").c_str(), &plane.showPlaneNormal);
		}
	}

	void RenderRayUI(Ray& ray, std::string const& rayName)
	{
		if (ImGui::CollapsingHeader(("Settings for " + rayName).c_str()))
		{
			ImGui::DragFloat3(("Color (" + rayName + ")").c_str(), (float*)&ray.model.color, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat3(("Point (" + rayName + ")").c_str(), (float*)&ray.startPoint, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat(("Length (" + rayName + ")").c_str(), (float*)&ray.length, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat3(("Rotation (" + rayName + ")").c_str(), (float*)&ray.model.rotDegrees, 1.0f, 360.0f, 30.0f);

			static std::string dirText = rayName + "'s direction vector";
			ImGui::Text(dirText.c_str());
			ImGui::Text("%f, %f, %f", ray.direction.x, ray.direction.y, ray.direction.z);
		}
	}

	void RenderKinematics(Kinematics& kinematics, std::string const& kinematicsName)
	{
		if (ImGui::CollapsingHeader(("Physics for " + kinematicsName).c_str()))
		{
			ImGui::Text("Direction is normalized every physics update");
			ImGui::DragFloat3(("direction (" + kinematicsName + ")").c_str(), (float*)&kinematics.normVector, 0.01f, -1.0f, 1.0f);
			ImGui::DragFloat(("speed (" + kinematicsName + ")").c_str(), (float*)&kinematics.speed, 0.01f, -10.0f, 10.0f);
		}
	}

	void RenderTriangleUI(Triangle& triangle, std::string const& triangleName)
	{
		if (ImGui::CollapsingHeader(("Settings for " + triangleName).c_str()))
		{

			ImGui::DragFloat(("pt0 x (" + triangleName + ")").c_str(), (float*)&triangle.pt0.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat(("pt0 y (" + triangleName + ")").c_str(), (float*)&triangle.pt0.y, 0.01f, -10.0f, 10.0f);


			ImGui::DragFloat(("pt1 x (" + triangleName + ")").c_str(), (float*)&triangle.pt1.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat(("pt1 y (" + triangleName + ")").c_str(), (float*)&triangle.pt1.y, 0.01f, -10.0f, 10.0f);

			ImGui::DragFloat(("pt2 x (" + triangleName + ")").c_str(), (float*)&triangle.pt2.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat(("pt2 y (" + triangleName + ")").c_str(), (float*)&triangle.pt2.y, 0.01f, -10.0f, 10.0f);
		}
	}
}

//Debug Testing
namespace Assignment
{
	namespace Tests
	{

		//Assignment One Tests
		namespace AssignmentOneTests
		{
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
				glm::vec3 relativeUp = glm::vec3(0.0f, 0.0f, -1.0f);

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
			void TestSphereAngle1()
			{
				//Basic test for a sphere with 4 stacks
				size_t numStacks = 4;

				auto approximateEplisionCheck = [](float lhs, float rhs)
				{
					return fabs(lhs - rhs) <= std::numeric_limits<float>::epsilon();
				};


				auto checkStack = [&](size_t currSlice, float expectedDegrees)
				{
					float actual = MeshConstructor::Sphere::getThetaDegrees(currSlice, numStacks);
					return (approximateEplisionCheck(actual, expectedDegrees));
				};

				assert(checkStack(0, 270.0f));
				assert(checkStack(1, 270.0f + 45.0f));
				assert(checkStack(2, 270.0f + 90.0f));
				assert(checkStack(3, 270.0f + 135.0f));
				assert(checkStack(4, 270.0f + 180.0f));
			}
			void TestSphereAngle2()
			{
				//Test the phi side

				//Basic test for a sphere with 4 slices
				size_t numSlices = 4;

				//Getting the angle of slices are very similar to doing a circle 

				auto approximateEplisionCheck = [](float lhs, float rhs)
				{
					return fabs(lhs - rhs) <= std::numeric_limits<float>::epsilon();
				};


				auto checkSlice = [&](size_t currSlice, float expectedDegrees)
				{
					float actual = MeshConstructor::Sphere::getPhiDegrees(currSlice, numSlices);
					return (approximateEplisionCheck(actual, expectedDegrees));
				};


				assert(checkSlice(0, 0.0f));
				assert(checkSlice(1, 90.0f));
				assert(checkSlice(2, 180.0f));
				assert(checkSlice(3, 270.0f));
				assert(checkSlice(4, 360.0f));
			}
			void TestSphereAngle3()
			{
				//Test the supposed mid section for 
				// totalStack = 5. currStack = 2.
				// totalSlice = 4. Loop thorugh all slice (draw from left to right)

				size_t numStacks = 3;
				size_t currStack = 1;

				size_t numSlices = 4;

				auto approximateEplisionCheck = [](float lhs, float rhs)
				{
					return fabs(lhs - rhs) <= std::numeric_limits<float>::epsilon();
				};

				auto checkSlice = [&](size_t currSlice, float expectedDegrees)
				{
					float actual = MeshConstructor::Sphere::getPhiDegrees(currSlice, numSlices);
					return (approximateEplisionCheck(actual, expectedDegrees));
				};

				auto checkStack = [&](size_t currSlice, float expectedDegrees)
				{
					float actual = MeshConstructor::Sphere::getThetaDegrees(currSlice, numStacks);
					return (approximateEplisionCheck(actual, expectedDegrees));
				};

				std::vector<glm::vec2> anglePairs;

				//Each slice has a 'pair' for the stack
				for (size_t currSlice = 0; currSlice <= numSlices; ++currSlice)
				{
					//Stacks must alternative
					float thisPhi = MeshConstructor::Sphere::getPhiDegrees(currSlice, numSlices);
					float stackOne = MeshConstructor::Sphere::getThetaDegrees(currStack, numStacks);
					float stackTwo = MeshConstructor::Sphere::getThetaDegrees(currStack + 1, numStacks);

					anglePairs.push_back(glm::vec2(stackOne, thisPhi));
					anglePairs.push_back(glm::vec2(stackTwo, thisPhi));
				}

				std::vector<glm::vec2> expectedAnglePairs =
				{
					glm::vec2(60.0f + 270.0f, 0.0f),
					glm::vec2(120.0f + 270.0f, 0.0f),
					glm::vec2(60.0f + 270.0f, 90.0f),
					glm::vec2(120.0f + 270.0f, 90.0f),
					glm::vec2(60.0f + 270.0f, 180.0f),
					glm::vec2(120.0f + 270.0f, 180.0f),
					glm::vec2(60.0f + 270.0f, 270.0f),
					glm::vec2(120.0f + 270.0f, 270.0f),
					glm::vec2(60.0f + 270.0f, 360.0f),
					glm::vec2(120.0f + 270.0f, 360.0f)
				};

				std::function<bool(glm::vec2, glm::vec2)> checkPair = [&](glm::vec2 lhs, glm::vec2 rhs)
				{
					return (approximateEplisionCheck(lhs.x, rhs.x) == (approximateEplisionCheck(lhs.y, rhs.y) == true));
				};

				assert(std::equal(expectedAnglePairs.begin(), expectedAnglePairs.end(), anglePairs.begin(), anglePairs.end(),
					checkPair));
			}
			void TestPointAABB()
			{
				glm::vec3 point = glm::vec3(1.0f, 0.0f, 0.0f);

				AABB aabb;
				aabb.centerPos = glm::vec3(0.0f, 0.0f, 0.0f);
				aabb.scale = glm::vec3(10.0f, 10.0f, 10.0f);

				aabb.CalculatePoints();

				assert(checkPointOnAABB(point, aabb));
			}
			void TestPointAABB2()
			{
				glm::vec3 point = glm::vec3(1.0f, 1000.0f, 0.0f);

				AABB aabb;
				aabb.centerPos = glm::vec3(0.0f, 0.0f, 0.0f);
				aabb.scale = glm::vec3(10.0f, 10.0f, 10.0f);

				aabb.CalculatePoints();

				assert(!checkPointOnAABB(point, aabb));
			}
			void TestSphereAABB()
			{
				SphereCollider sphere;
				sphere.centerPos = glm::vec3(1.0f, 1.0f, 1.0f);
				sphere.radius = 1.0f;

				//Should collide
				AABB aabb;
				aabb.centerPos = glm::vec3(1.0f, 2.0f, 1.0f);
				aabb.scale = glm::vec3(10.0f, 10.0f, 10.0f);

				aabb.CalculatePoints();

				//Must be true
				assert(collisionCheck(aabb, sphere));
			}
			void TestAABBvsAABB()
			{
				AABB aabb;
				aabb.centerPos = glm::vec3(1.0f, 2.0f, 1.0f);
				aabb.scale = glm::vec3(10.0f, 10.0f, 10.0f);

				aabb.CalculatePoints();

				AABB aabb2;
				aabb2.centerPos = glm::vec3(0.0f, 0.0f, 1.0f);
				aabb2.scale = glm::vec3(10.0f, 10.0f, 10.0f);

				aabb2.CalculatePoints();

				assert(collisionCheck(aabb, aabb2));
			}
			void TestAABBvsAABB2()
			{
				AABB aabb;
				aabb.centerPos = glm::vec3(1.0f, 100.0f, 1.0f);
				aabb.scale = glm::vec3(10.0f, 10.0f, 10.0f);

				aabb.CalculatePoints();

				AABB aabb2;
				aabb2.centerPos = glm::vec3(0.0f, 0.0f, 1.0f);
				aabb2.scale = glm::vec3(1.0f, 1.0f, 1.0f);

				aabb2.CalculatePoints();

				assert(!collisionCheck(aabb, aabb2));
			}
			void TestPointOnPlane()
			{
				Plane plane;
				plane.pointOnPlane = glm::vec3(0.0f, 0.0f, 0.0f);
				plane.outwardNormal = glm::vec3(0.0f, 1.0f, 0.0f); //This is the floor

				//Should count
				glm::vec3 pointOne = glm::vec3(100.0f, 0.0f, 0.0f);
				glm::vec3 pointTwo = glm::vec3(100.0f, 0.0f, 100.0f);

				assert(checkPointOnPlane(pointOne, plane));
				assert(checkPointOnPlane(pointTwo, plane));

				//Should reject
				glm::vec3 pointThree = glm::vec3(100.0f, 1.0f, 0.0f);
				assert(!checkPointOnPlane(pointThree, plane));

			}
			void TestPointOnPlane2()
			{
				Plane plane;
				plane.pointOnPlane = glm::vec3(30.0f, 10.0f, 0.0f);
				plane.outwardNormal = glm::vec3(1.0f, 1.0f, 1.0f); //Some diagional plane

				//Should count
				glm::vec3 pointOne = glm::vec3(30.0f, 10.0f, 0.0f);
				assert(checkPointOnPlane(pointOne, plane));

				glm::vec3 pointTwo = glm::vec3(10.0f, 50.0f, 100.0f);
				assert(!checkPointOnPlane(pointTwo, plane));

			}
			void TestPointOnPlane3()
			{
				Plane plane;
				plane.pointOnPlane = glm::vec3(0.0f, 10.0f, 0.0f);
				plane.outwardNormal = glm::vec3(0.0f, 0.0f, 1.0f);

				//Should count
				glm::vec3 pointOne = glm::vec3(0.0f, 10.0f, 0.0f);
				assert(checkPointOnPlane(pointOne, plane));

				//Should also count
				glm::vec3 pointTwo = glm::vec3(0.0f, 50.0f, 0.0f);
				assert(checkPointOnPlane(pointTwo, plane));

				//Point is front of the plane now
				glm::vec3 pointThree = glm::vec3(0.0f, 50.0f, 1.0f);
				assert(!checkPointOnPlane(pointThree, plane));
			}
			void TestRayOnSphere()
			{
				//Test for trivial acceptance
				SphereCollider sphere;
				Ray ray;


				//Ray is definitely inside (start is inside)
				sphere.radius = 1.0f;
				sphere.centerPos = glm::vec3(0.0f, 0.0f, 0.0f);

				ray.startPoint = glm::vec3(0.0f, 0.0f, 0.0f);
				ray.length = 1.0f;
				ray.direction = glm::vec3(1.0f, 0.0f, 0.0f);

				assert(checkRayOnSphere(ray, sphere));

				///Ray is inside (end is inside)
				ray.startPoint = glm::vec3(-2.0f, 0.0f, 0.0f);
				ray.length = 2.1f; //Ray ends at origin
				ray.direction = glm::vec3(1.0f, 0.0f, 0.0f);

				assert(checkRayOnSphere(ray, sphere));

				//Test for trivial rejection away
				ray.startPoint = glm::vec3(-2.0f, 0.0f, 0.0f);
				ray.direction = glm::vec3(-1.0f, 0.0f, 0.0f);
				assert(!checkRayOnSphere(ray, sphere));

				//Test for skimmed past
				ray.startPoint = glm::vec3(-2.0f, -20.0f, 0.0f);
				ray.length = 100.0f; //Ray ends at origin
				ray.direction = glm::vec3(1.0f, 0.0f, 0.0f);
				assert(!checkRayOnSphere(ray, sphere));

				//Test for intersection true
				ray.startPoint = glm::vec3(-20.0f, 0.0f, 0.0f);
				ray.length = 100.0f; //Ray ends at origin
				ray.direction = glm::vec3(1.0f, 0.0f, 0.0f);
				assert(checkRayOnSphere(ray, sphere));


			}
			void TestPointOnTriangle()
			{
				glm::vec3 pt = glm::vec3(0.0f, 0.0f, 0.0f);

				Triangle triangle;
				//Surrounds the origin
				triangle.pt0 = glm::vec3(0.0f, 1.0f, 0.0f);
				triangle.pt1 = glm::vec3(-1.0f, 0.0f, 0.0f);
				triangle.pt2 = glm::vec3(1.0f, 0.0f, 0.0f);

				assert(checkPointOnTriangle(triangle, pt));

				//Definitely not on the triangle
				pt = glm::vec3(0.0f, 10.0f, 0.0f);
				assert(!checkPointOnTriangle(triangle, pt));
			}


			void RunTests()
			{
				TestQuadVertices1();
				TestQuadVertices2();
				TestQuadVertices3();
				TestQuadVertices4();
				TestSphereAngle1();
				TestSphereAngle2();
				TestSphereAngle3();

				TestPointAABB();
				TestPointAABB2();

				TestSphereAABB();

				TestAABBvsAABB();
				TestAABBvsAABB2();

				TestPointOnPlane();
				TestPointOnPlane2();
				TestPointOnPlane3();
				TestRayOnSphere();

				TestPointOnTriangle();
			}
		}

		namespace AssignmentTwoTests
		{
			//Assignment Two Tests
			void TestAABBCalculation()
			{
				//Check with a 3D pyramid-like structure
				std::vector<glm::vec3> transformList;

				//The square base
				transformList.push_back(glm::vec3(-1.0f, 0.0f, -1.0f));
				transformList.push_back(glm::vec3(1.0f, 0.0f, -1.0f));
				transformList.push_back(glm::vec3(-1.0f, 0.0f, 1.0f));
				transformList.push_back(glm::vec3(1.0f, 0.0f, 1.0f));

				//The tip of the pyramid
				transformList.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

				BV::AABB aabb;

				BV::CalculateAABB(transformList, aabb);

				glm::vec3 expectedMaxPoint = glm::vec3(1.0f, 1.0f, 1.0f);
				assert(expectedMaxPoint == aabb.maxPoint);

				glm::vec3 expectedMinPoint = glm::vec3(-1.0f, 0.0f, -1.0f);
				assert(expectedMinPoint == aabb.minPoint);
			}

			void RunTests()
			{
				TestAABBCalculation();
			}
		}
		void RunAllTests()
		{
			//AssignmentOneTests::RunTests();
			AssignmentTwoTests::RunTests();
		}
	}

}


//Scenes
namespace Assignment
{
	//Default init for a camera
	void initCamera()
	{
		currCamera.pos = defaultCameraPos;
	}
	void initBackground()
	{
		backgroundColor = neutralBackgroundColor;
		prevBackGround = backgroundColor;
	}

	void initFresh()
	{
		initCamera();
		initBackground();
	}

	//Apply some simple polymorphism in order to create helper function for adding scenes to the map
	class Scene
	{
	public:
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
	};

	class TestScene : Scene
	{
	public:
		void Init() override { std::cout << "Init()\n"; };
		void Update() override { std::cout << "Update()\n"; };
		void Render() override { std::cout << "Render()\n"; };
	};

	class CubeScene : Scene
	{
	public:
		Transform cubeModel;

		void Init() override {
			cubeModel.scale = glm::vec3(1.0f, 1.0f, 1.0f);
		}

		void Update() override {
			cubeModel.rotDegrees.y += applicationPtr->getDeltaTime() * 200.0f;
		}

		void Render() override {
			RenderAxis();

			SubmitDraw(cubeModel, MeshNames::cube);
			DrawAll(drawList, currCamera);
			RenderPictureinPicture();
			drawList.clear();
		}
	};

	class AssimapExample : Scene
	{
	public:
		std::vector<Object> objectVector;
		std::vector<std::string> filePaths;
		std::string modelFolderPath = "Models/";
		BV::AABB aabbTest;

		void Init() override {

			LoadScene(objectVector, "Scenes/AssignmentTwo.txt");
			//std::vector<glm::vec3> v = BV::GetObjectPositions(objectVector, 0, objectVector.size() - 1);
			//BV::CalculateAABB(v, aabbTest);
			

			//for (std::string const& path : filePaths)
			//{
			//	Object obj;
			//	obj.objectMesh.loadOBJ(path);
			//	objectVector.push_back(obj);
			//}
		}

		void Update() override {
			//cubeTransforms.rotDegrees.y += applicationPtr->getDeltaTime() * 200.0f;
		}

		void Render() override
		{
			RenderAxis();

			for (const Object& obj : objectVector)
			{
				SubmitDraw(obj.transform, obj.objectMesh.meshBuffer);
			}

			DrawAll(drawList, currCamera);
			//RenderPictureinPicture();
			drawList.clear();
		}
	};

	//Convert the specified scene into functions and adds it to sceneMap
	template <typename T>
	void ScenetoFunction(std::string const& sceneName)
	{
		//static unique_ptr allows runtime polymorphism with <memory> managing it automatically
		static std::unique_ptr<T> scenePtr = std::make_unique<T>();

		AssignmentScene curr;
		curr.initScene = std::bind(&T::Init, scenePtr.get());
		curr.updateScene = std::bind(&T::Update, scenePtr.get());
		curr.renderScene = std::bind(&T::Render, scenePtr.get());

		sceneMap.insert(std::make_pair<std::string, AssignmentScene>(sceneName.c_str(), AssignmentScene(curr)));
	}

	//Probably gonna only be one scene this time with options to toggle 
	namespace AssignmentTwoScenes
	{
		class BaseScene : Scene
		{
		protected:
			//Every scene for A2 have these functions
			std::vector<Object> objectVector;



			std::vector<std::string> filePaths;
			std::string modelFolderPath = "Models/";

			void RenderObjects()
			{
				for (const Object& obj : objectVector)
				{
					SubmitDraw(obj.transform, obj.objectMesh.meshBuffer);
				}

				DrawAll(drawList, currCamera);
				drawList.clear();
			}

		};

		//Test if I put a few cubes, if it'd generate an expected AABB and rendering
		class CubeLoadScene : public BaseScene
		{
			BV::AABB aabbAll;
		public:
			void Init() override {
				LoadScene(objectVector, "Scenes/CubeTest.txt");
				
				std::vector<glm::vec3> v = BV::GetObjectPositions(objectVector);
				BV::CalculateAABB(v, aabbAll);
			}

			void Update() override {

			}

			void Render() override
			{
				RenderAxis();
				RenderObjects();
			}
		};

		class AssignmentTwoScene : public BaseScene
		{
		public:
			void Init() override {

			}

			void Update() override {

			}

			void Render() override
			{

			}

		};
	}

	namespace AssignmentOneScenes
	{
		//AssignmentOneScenes that does nothing
		namespace Default
		{
			void Init() {};
			void Update() {};
			void Render() {};
		}

		namespace Cube
		{
			Transform cubeModel;

			//Assumption: Meshes are already initialized so we are only creating models
			void Init()
			{
				cubeModel.scale = glm::vec3(1.0f, 1.0f, 1.0f);
			}

			void Update()
			{
				cubeModel.rotDegrees.y += applicationPtr->getDeltaTime() * 200.0f;
			}


			//The DearImgui for this scene
			void RenderDearImgui()
			{
				RenderDearImguiDefault();
			}

			void Render()
			{
				RenderAxis();

				SubmitDraw(cubeModel, MeshNames::cube);
				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}
		}

		namespace Sphere
		{
			Transform sphereModel;

			void Init()
			{

				currCamera.pos = defaultLookAtCamera.pos;
				sphereModel.scale = glm::vec3(1.0f, 1.0f, 1.0f);
			}

			void Update()
			{
				sphereModel.rotDegrees.y += applicationPtr->getDeltaTime() * 20.0f;
			}

			void Render()
			{
				RenderAxis();

				SubmitDraw(sphereModel, MeshNames::sphere);
				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}

		}

		namespace AABBVsSphere
		{
			SphereCollider sphere;
			AABB box;
			Kinematics boxKinematics;

			glm::vec3 boxStartPos = glm::vec3(0.0f, 1.0f, -10.0f);
			glm::vec3 boxStartScale = glm::vec3(2.0f, 2.0f, 2.0f);

			float boxStartSpeed = 3.0f;
			glm::vec3 boxVelocityNorm = worldForward;

			void Init()
			{
				currCamera.pos = defaultCameraPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				box.model.color = greenscreenGreen;
				box.centerPos = boxStartPos;
				box.scale = boxStartScale;

				boxKinematics.speed = boxStartSpeed;
				boxKinematics.normVector = worldForward;

				sphere.centerPos = worldOrigin;
				sphere.radius = 0.25f;
				sphere.model.color = coolOrange;

			}


			void RenderSettings()
			{
				ImGui::Begin("AABB vs Sphere Settings");

				RenderAABBUI(box, "AABB");
				RenderKinematics(boxKinematics, "AABB Speed");

				RenderSphereUI(sphere, "Sphere");
				ImGui::End();
			}

			void Update()
			{
				RenderSettings();

				UpdatePhysics(box.centerPos, boxKinematics);
				box.CalculatePoints();
				box.UpdateModel();

				sphere.UpdateModel();

				if (collisionCheck(box, sphere))
				{
					collisionDetected = true;
					box.model.color = maroonColor;
					sphere.model.color = greenscreenGreen;
					backgroundColor = collidedBackgroundColor;
					collisionDetected = true;
				}
				else
				{
					collisionDetected = false;
					box.model.color = greenscreenGreen;
					sphere.model.color = coolOrange;

					if (backgroundColor == collidedBackgroundColor)
						backgroundColor = neutralBackgroundColor;
				}
			};

			void Render()
			{
				RenderAxis();

				SubmitDraw(box.model, box.meshID, colorShader.shaderName);
				SubmitDraw(sphere.model, sphere.meshID, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();

			};
		}

		namespace SphereVsAABB
		{
			//Same but I make the Sphere move instead I guess
			SphereCollider sphere;
			AABB box;
			Kinematics sphereKinematics;

			glm::vec3 boxStartPos = worldOrigin;
			glm::vec3 boxStartScale = glm::vec3(2.0f, 2.0f, 2.0f);

			glm::vec3 sphereStartPos = glm::vec3(0.0f, 1.0f, -10.0f);

			float sphereStartSpeed = 3.0f;
			glm::vec3 sphereVelocityNorm = worldForward;

			void Init()
			{
				currCamera.pos = defaultCameraPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				box.model.color = greenscreenGreen;
				box.centerPos = boxStartPos;
				box.scale = boxStartScale;

				sphereKinematics.speed = sphereStartSpeed;
				sphereKinematics.normVector = worldForward;

				sphere.centerPos = sphereStartPos;
				sphere.radius = 0.25f;
				sphere.model.color = coolOrange;

			}


			void RenderSettings()
			{
				ImGui::Begin("Sphere vs AABB Settings");

				RenderSphereUI(sphere, "Sphere");
				RenderKinematics(sphereKinematics, "Sphere Speed");

				RenderAABBUI(box, "AABB");

				ImGui::End();
			}

			void Update()
			{
				RenderSettings();

				UpdatePhysics(sphere.centerPos, sphereKinematics);
				box.CalculatePoints();
				box.UpdateModel();

				sphere.UpdateModel();

				if (collisionCheck(box, sphere))
				{
					collisionDetected = true;
					box.model.color = maroonColor;
					sphere.model.color = greenscreenGreen;
					backgroundColor = collidedBackgroundColor;
				}
				else
				{

					box.model.color = greenscreenGreen;
					sphere.model.color = coolOrange;

					if (backgroundColor == collidedBackgroundColor)
						backgroundColor = neutralBackgroundColor;
				}
			};

			void Render()
			{
				RenderAxis();

				SubmitDraw(box.model, box.meshID, colorShader.shaderName);
				SubmitDraw(sphere.model, sphere.meshID, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();

			};
		}

		namespace SphereVsSphere
		{
			//GameObject sphereObject;

			SphereCollider sphereOne;
			SphereCollider sphereTwo;
			Kinematics sphereOnePhysics;

			glm::vec3 sphereOneStartPos = glm::vec3(0.0f, 1.0f, -10.0f);
			float sphereOneStartSpeed = 2.0f;
			glm::vec3 sphereOneVelocityDir = worldForward;

			//SphereCollider sphereTwo;

			void Init()
			{
				sphereOne.radius = 0.25f;
				sphereOne.centerPos = sphereOneStartPos;
				sphereOnePhysics.speed = sphereOneStartSpeed;
				sphereOnePhysics.normVector = sphereOneVelocityDir;
				sphereOne.model.color = basicBlue;

				sphereTwo.radius = 1.0f;
				sphereTwo.centerPos = glm::vec3(0.0f, 1.0f, 0.0f);
				sphereTwo.model.color = greenscreenGreen;

				currCamera.pos = defaultCameraPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;
			}

			void RenderSettings()
			{
				ImGui::Begin("Sphere vs Sphere Settings");

				RenderSphereUI(sphereOne, "Sphere One");
				RenderKinematics(sphereOnePhysics, "Sphere One Speed");

				RenderSphereUI(sphereTwo, "Sphere Two");
				ImGui::End();
			}

			void Update()
			{
				RenderSettings();

				UpdatePhysics(sphereOne.centerPos, sphereOnePhysics);

				if (collisionCheck(sphereOne, sphereTwo))
				{
					collisionDetected = true;
					sphereOne.model.color = coolOrange;
					sphereTwo.model.color = coolPurpleColor;

					backgroundColor = collidedBackgroundColor;
				}
				else
				{
					sphereTwo.model.color = greenscreenGreen;
					sphereOne.model.color = basicBlue;
					if (backgroundColor == collidedBackgroundColor)
						backgroundColor = neutralBackgroundColor;
				}


				sphereOne.UpdateModel();
				sphereTwo.UpdateModel();
			}

			void Render()
			{


				RenderAxis();
				SubmitDraw(sphereOne.model, sphereOne.meshID, colorShader.shaderName);
				SubmitDraw(sphereTwo.model, sphereTwo.meshID, colorShader.shaderName);
				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}

		}

		namespace AABBvsAABB
		{
			AABB box;
			AABB boxTwo;
			Kinematics boxKinematics;

			glm::vec3 boxStartPos = glm::vec3(0.0f, 1.0f, -15.0f);
			glm::vec3 boxTwoStartPos = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 boxStartScale = glm::vec3(5.0f, 5.0f, 5.0f);
			glm::vec3 boxTwoStartScale = glm::vec3(10.0f, 1.0f, 1.0f); //a little thin
			float boxStartSpeed = 2.0f;
			glm::vec3 boxVelocityNorm = worldForward;

			void Init()
			{
				currCamera.pos = defaultCameraPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				box.model.color = greenscreenGreen;
				box.centerPos = boxStartPos;
				box.scale = boxStartScale;

				boxKinematics.speed = boxStartSpeed;
				boxKinematics.normVector = worldForward;

				boxTwo.centerPos = boxTwoStartPos;
				boxTwo.scale = boxTwoStartScale;

			}

			void RenderSettings()
			{
				ImGui::Begin("AABB vs AABB Settings");

				RenderAABBUI(box, "AABB One");
				RenderKinematics(boxKinematics, "AABB One Speed");

				RenderAABBUI(boxTwo, "AABB Two");
				ImGui::End();
			}

			void Update()
			{
				RenderSettings();

				UpdatePhysics(box.centerPos, boxKinematics);
				box.CalculatePoints();
				box.UpdateModel();

				boxTwo.CalculatePoints();
				boxTwo.UpdateModel();

				if (collisionCheck(box, boxTwo))
				{
					collisionDetected = true;
					box.model.color = coolPurpleColor;
					boxTwo.model.color = skyBlue;
					backgroundColor = collidedBackgroundColor;
				}
				else
				{
					box.model.color = greenscreenGreen;
					boxTwo.model.color = coolOrange;

					if (backgroundColor == collidedBackgroundColor)
						backgroundColor = neutralBackgroundColor;
				}
			};

			void Render()
			{
				RenderAxis();

				SubmitDraw(box.model, box.meshID, colorShader.shaderName);
				SubmitDraw(boxTwo.model, box.meshID, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();

			};
		}

		namespace PointVsSphere
		{
			//GameObject sphereObject;

			SphereCollider sphereOne;
			Kinematics sphereOnePhysics;

			glm::vec3 sphereOneStartPos = glm::vec3(0.0f, 1.0f, -10.0f);
			float sphereOneStartSpeed = 2.0f;
			glm::vec3 sphereOneVelocityDir = worldForward;

			Transform pointModel;
			glm::vec3 pointCollision = glm::vec3(0.0f, 1.0f, 0.0f);

			//For the physical representation. Does not influence collision
			glm::vec3 pointModelScale = glm::vec3(0.05f, 0.05f, 0.05f);

			void Init()
			{


				pointModel.pos = pointCollision;
				pointModel.scale = pointModelScale;

				sphereOne.radius = 0.25f;
				sphereOne.centerPos = sphereOneStartPos;
				sphereOnePhysics.speed = sphereOneStartSpeed;
				sphereOnePhysics.normVector = sphereOneVelocityDir;
				sphereOne.model.color = basicBlue;

				currCamera.pos = defaultCameraPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;
			}

			void RenderSettings()
			{
				ImGui::Begin("Point vs Sphere Settings");

				RenderSphereUI(sphereOne, "Sphere");
				RenderKinematics(sphereOnePhysics, "Sphere Speed");

				ImGui::DragFloat3("Point Pos ", (float*)&pointCollision, 0.01f, -20.0f, 20.0f);
				ImGui::End();
			}

			void Update()
			{
				RenderSettings();

				UpdatePhysics(sphereOne.centerPos, sphereOnePhysics);

				if (checkPointOnSphere(pointCollision, sphereOne))
				{
					collisionDetected = true;
					sphereOne.model.color = coolOrange;
					backgroundColor = collidedBackgroundColor;
				}
				else
				{
					sphereOne.model.color = basicBlue;
					backgroundColor = neutralBackgroundColor;
				}

				pointModel.pos = pointCollision;
				sphereOne.UpdateModel();
			}

			void Render()
			{


				RenderAxis();
				SubmitDraw(sphereOne.model, sphereOne.meshID, colorShader.shaderName);
				SubmitDraw(pointModel, MeshNames::sphere, colorShader.shaderName);
				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}

		}

		namespace PointVsAABB
		{
			AABB box;
			Kinematics boxKinematics;

			//Can use a sphere to represent it
			Transform pointModel;
			glm::vec3 pointCollision = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::vec3 boxStartPos = glm::vec3(0.0f, 1.0f, -5.0f);
			glm::vec3 boxStartScale = glm::vec3(1.0f, 1.0f, 1.0f);

			float boxStartSpeed = 2.0f;
			glm::vec3 boxVelocityNorm = worldForward;

			//For the physical representation. Does not influence collision
			glm::vec3 pointModelScale = glm::vec3(0.05f, 0.05f, 0.05f);

			void Init()
			{
				currCamera.pos = defaultCameraPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				box.model.color = greenscreenGreen;
				box.centerPos = boxStartPos;
				box.scale = boxStartScale;

				boxKinematics.speed = boxStartSpeed;
				boxKinematics.normVector = worldForward;

				pointModel.pos = pointCollision;
				pointModel.scale = pointModelScale;
			}

			void RenderSettings()
			{
				ImGui::Begin("Point vs AABB Settings");

				RenderAABBUI(box, "AABB");
				RenderKinematics(boxKinematics, "AABB Speed");

				ImGui::DragFloat3("Point", (float*)&pointCollision, 0.01f, -20.0f, 20.0f);
				ImGui::End();
			}

			void Update()
			{
				RenderSettings();

				UpdatePhysics(box.centerPos, boxKinematics);
				box.CalculatePoints();
				box.UpdateModel();

				pointModel.pos = pointCollision;

				if (checkPointOnAABB(pointCollision, box))
				{
					collisionDetected = true;
					box.model.color = skyBlue;
					backgroundColor = collidedBackgroundColor;
				}
				else
				{
					box.model.color = greenscreenGreen;

					if (backgroundColor == collidedBackgroundColor)
						backgroundColor = neutralBackgroundColor;
				}
			};

			void Render()
			{
				RenderAxis();

				SubmitDraw(box.model, box.meshID, colorShader.shaderName);
				SubmitDraw(pointModel, MeshNames::sphere, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();

			};
		}

		namespace PointVsPlane
		{
			Plane plane;
			glm::vec3 cameraStartPos = glm::vec3(1.0f, 1.0f, 1.0f);

			Kinematics pointKinematics;

			Transform pointModel;
			glm::vec3 pointModelScale = glm::vec3(0.01f, 0.01f, 0.01f);

			glm::vec3 pointCollision = glm::vec3(0.0f, 0.1f, 0.0f);

			float rotateSpeed = 10.0f;

			float currDistanceFromPlane = -1.11f;

			float pointStartSpeed = 1.0f;
			glm::vec3 boxVelocityNorm = worldForward;

			//For the physical representation. Does not influence collision


			void Init()
			{
				//Default for this scene is a little different 
				changeBackGroundOnCollision = false;
				showAxis = false;
				wireFrameMode = false;

				//Closer for this scene
				topDownCameraHeight = 1.0f;

				currCamera.pos = cameraStartPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				plane.outwardNormal = worldRight;
				pointModel.color = coolOrange;


				plane.model.scale = glm::vec3(1.0f, 1.0f, 1.0f);
				plane.pointOnPlane = glm::vec3(0.0f, 0.0f, 0.0f);
				plane.normalModel.color = skyBlue;
				plane.normalModel.scale = glm::vec3(1.0f, 1.0f, 10.0f);



				pointModel.pos = pointCollision;
				pointModel.scale = pointModelScale;

			}

			void RenderSettings()
			{
				ImGui::Begin("Plane vs Point Settings");
				ImGui::Spacing();

				ImGui::Text(("Point nearest distance from plane: " + std::to_string(currDistanceFromPlane)).c_str());
				ImGui::Spacing();

				RenderPlaneUI(plane, "Plane");
				//ImGui::DragFloat("Plane Rotation Speed", (float*)&rotateSpeed, 90.0f, 0.0f, 360.0f);
				ImGui::DragFloat3("Point", (float*)&pointCollision, 0.01f, -100.0f, 100.0f);

				ImGui::End();
			}

			void Update()
			{
				//UpdatePhysics(pointCollision, pointKinematics);

				pointModel.pos = pointCollision;
				currDistanceFromPlane = distanceFromPlane(pointCollision, plane);

				RenderSettings();
				UpdatePlane(plane);

				if (checkPointOnPlane(pointCollision, plane))
				{
					collisionDetected = true;
					plane.model.color = glm::vec3(1.0f, 0.0f, 0.0f); //Red
					//backgroundColor = collidedBackgroundColor;
				}
				else
				{
					plane.model.color = glm::vec3(1.0f, 1.0f, 1.0f);

					//if (backgroundColor == collidedBackgroundColor)
					//	//backgroundColor = neutralBackgroundColor;
				}

			}

			void Render()
			{
				RenderAxis();

				SubmitDraw(plane.model, MeshNames::quadNormalRight, colorShader.shaderName);

				if (plane.showPlaneNormal)
					SubmitDraw(plane.normalModel, MeshNames::rayRight, colorShader.shaderName);

				SubmitDraw(pointModel, MeshNames::sphere, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}

		}

		namespace PlaneVsSphere
		{
			Plane plane;
			SphereCollider sphereOne;

			Kinematics sphereOnePhysics;
			Kinematics planePhysics;


			glm::vec3 sphereOneStartPos = glm::vec3(5.0f, 1.0f, 0.0f);
			float sphereOneStartSpeed = 1.0f;
			glm::vec3 sphereOneVelocityDir = -worldRight;

			float currDistanceFromPlane = -1.11f;

			void Init()
			{
				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				//Default for this scene is a little different 
				changeBackGroundOnCollision = false;
				showAxis = false;
				wireFrameMode = false;


				currCamera.pos = glm::vec3(4.0f, 4.0f, 4.0f);
				topDownCameraHeight = 5.0f;

				plane.outwardNormal = worldRight;
				plane.model.scale = glm::vec3(1.0f, 3.0f, 3.0f);
				plane.pointOnPlane = glm::vec3(0.0f, 0.0f, 0.0f);
				plane.normalModel.color = skyBlue;
				plane.normalModel.scale = glm::vec3(1.0f, 1.0f, 10.0f);
				plane.rotation = glm::vec3(0.0f, 0.0f, 45.0f);

				planePhysics.normVector = glm::vec3(0.0f, 0.0f, 1.0f);
				planePhysics.speed = -55.0f;


				sphereOne.centerPos = sphereOneStartPos;
				sphereOne.radius = 0.25f;
				sphereOne.centerPos = sphereOneStartPos;
				sphereOnePhysics.speed = sphereOneStartSpeed;
				sphereOnePhysics.normVector = sphereOneVelocityDir;
				sphereOne.model.color = basicBlue;





			}

			void RenderSettings()
			{
				ImGui::Begin("Plane vs Sphere Settings");
				ImGui::Spacing();

				ImGui::Text(("Point nearest distance from plane: " + std::to_string(currDistanceFromPlane)).c_str());
				ImGui::Spacing();

				RenderPlaneUI(plane, "Plane");
				RenderKinematics(planePhysics, "Plane Rotations");


				RenderSphereUI(sphereOne, "Sphere");
				RenderKinematics(sphereOnePhysics, "Sphere");

				ImGui::End();
			}

			void Update()
			{
				currDistanceFromPlane = distanceFromPlane(sphereOne.centerPos, plane);

				RenderSettings();

				UpdatePhysics(plane.rotation, planePhysics);
				UpdatePlane(plane);

				UpdatePhysics(sphereOne.centerPos, sphereOnePhysics);


				if (checkPlaneOnSphere(plane, sphereOne))
				{
					collisionDetected = true;
					sphereOne.model.color = greenscreenGreen;

					collisionDetected = true;
					plane.model.color = glm::vec3(1.0f, 0.0f, 0.0f);

					backgroundColor = collidedBackgroundColor;
				}
				else
				{
					plane.model.color = glm::vec3(1.0f, 1.0f, 1.0f);
					sphereOne.model.color = coolPurpleColor;
				}

				sphereOne.UpdateModel();
			}

			void Render()
			{
				RenderAxis();

				SubmitDraw(plane.model, MeshNames::quadNormalRight, colorShader.shaderName);
				if (plane.showPlaneNormal)
					SubmitDraw(plane.normalModel, MeshNames::rayRight, colorShader.shaderName);

				SubmitDraw(sphereOne.model, MeshNames::sphere, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}
		}

		namespace PlaneVsAABB
		{
			Plane plane;
			AABB box;

			Kinematics boxPhysics;
			Kinematics planePhysics;

			glm::vec3 boxStartPos = glm::vec3(5.0f, 0.0f, 0.0f);
			float boxSpeed = 1.0f;
			glm::vec3 boxVelocityDir = -worldRight;

			float currDistanceFromPlane = -1.11f;

			void Init()
			{
				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				//Default for this scene is a little different 
				changeBackGroundOnCollision = false;
				showAxis = false;
				wireFrameMode = false;


				currCamera.pos = glm::vec3(4.0f, 4.0f, 4.0f);
				topDownCameraHeight = 5.0f;

				plane.outwardNormal = worldRight;
				plane.model.scale = glm::vec3(1.0f, 3.0f, 3.0f);
				plane.pointOnPlane = glm::vec3(0.0f, 0.0f, 0.0f);
				plane.normalModel.color = skyBlue;
				plane.normalModel.scale = glm::vec3(1.0f, 1.0f, 10.0f);
				plane.rotation = glm::vec3(0.0f, 0.0f, 45.0f);

				planePhysics.normVector = glm::vec3(0.0f, 0.0f, 1.0f);
				planePhysics.speed = 0.0f;

				box.centerPos = boxStartPos;
				box.scale = glm::vec3(0.75f, 0.75f, 0.75f);
				boxPhysics.speed = boxSpeed;
				boxPhysics.normVector = boxVelocityDir;

			}

			void RenderSettings()
			{
				ImGui::Begin("Plane vs AABB Settings");
				ImGui::Spacing();

				ImGui::Text(("Point nearest distance from plane: " + std::to_string(currDistanceFromPlane)).c_str());
				ImGui::Spacing();

				RenderPlaneUI(plane, "Plane");
				RenderKinematics(planePhysics, "Plane Rotations");


				RenderAABBUI(box, "AABB");
				RenderKinematics(boxPhysics, "Sphere");

				ImGui::End();
			}

			void Update()
			{
				currDistanceFromPlane = distanceFromPlane(box.centerPos, plane);

				box.CalculatePoints();

				RenderSettings();

				UpdatePhysics(plane.rotation, planePhysics);
				UpdatePlane(plane);

				UpdatePhysics(box.centerPos, boxPhysics);
				box.UpdateModel();


				//Just check both the min and max points

				if (checkPlaneOnAABB(plane, box))
				{
					collisionDetected = true;
					box.model.color = greenscreenGreen;

					plane.model.color = glm::vec3(1.0f, 0.0f, 0.0f);

					backgroundColor = collidedBackgroundColor;
				}
				else
				{
					collisionDetected = false;
					plane.model.color = glm::vec3(1.0f, 1.0f, 1.0f);
					box.model.color = coolPurpleColor;
				}


			}

			void Render()
			{
				RenderAxis();

				SubmitDraw(plane.model, MeshNames::quadNormalRight, colorShader.shaderName);
				if (plane.showPlaneNormal)
					SubmitDraw(plane.normalModel, MeshNames::rayRight, colorShader.shaderName);

				SubmitDraw(box.model, MeshNames::cube, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}
		}

		namespace RayVsPlane
		{
			Ray ray;
			Plane plane;

			Kinematics rayRotations;

			std::string intersectionTimeString = "no intersection";

			void Init()
			{
				initFresh();
				currCamera.pos = glm::vec3(1.0f, 1.0f, 1.0f);

				changeBackGroundOnCollision = false;
				showAxis = false;
				wireFrameMode = false;

				ray.startPoint = glm::vec3(0.0f, 0.1f, 0.0f);
				ray.length = 1.0f;

				//RotateRay(ray, 0.0f, 0.0f, 0.0f);

				//Don't set this directly. Rotate to get to the norm (easier for the mvp)
				//ray.direction = glm::vec3(1.0f, 1.0f, 1.0f);

				ray.model.color = glm::vec3(0.0f, 1.0f, 1.0f);

				//Rotations are based on right vector transformations
				ray.meshID = MeshNames::rayRight;

				//The plane is behind the start point of the ray by default at first.
				plane.pointOnPlane = glm::vec3(-0.5f, 0.0f, 0.0f);
				plane.outwardNormal = worldRight;
				plane.rotation = glm::vec3(0.0f, 0.0f, 45.0f);
				plane.model.color = greenscreenGreen;
				plane.normalModel.color = coolPurpleColor;
				plane.model.scale = glm::vec3(1.0f, 1.4f, 1.4f);

				//rotate around y-axis
				rayRotations.normVector = glm::vec3(0.0f, 1.0f, 0.0f);
				rayRotations.speed = 25.0f;

			}

			void RenderSettings()
			{
				ImGui::Begin("Ray vs Plane Settings");

				ImGui::Text(intersectionTimeString.c_str());
				RenderPlaneUI(plane, "plane");
				RenderRayUI(ray, "ray");
				RenderKinematics(rayRotations, "ray rotation kinematics");

				ImGui::End();
			}

			void Update()
			{
				RenderSettings();
				UpdatePhysics(ray.model.rotDegrees, rayRotations);
				UpdateRay(ray);
				UpdatePlane(plane);

				if (checkRayOnPlane(ray, plane))
				{
					intersectionTimeString = ("intersection time at:" + std::to_string((getIntersectionTimeRayOnPlane(ray, plane))));

					plane.normalModel.color = basicBlue;
					plane.model.color = glm::vec3(1.0f, 0.0f, 0.0f);
				}
				else
				{
					intersectionTimeString = "no intersection";
					plane.model.color = greenscreenGreen;
					plane.normalModel.color = coolPurpleColor;
				}
			}

			void Render()
			{
				RenderAxis();

				SubmitDraw(ray.model, MeshNames::rayRight, colorShader.shaderName);

				SubmitDraw(plane.model, MeshNames::quadNormalRight, colorShader.shaderName);

				if (plane.showPlaneNormal)
					SubmitDraw(plane.normalModel, MeshNames::rayRight, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}
		}

		namespace RayVsSphere
		{
			Ray ray;
			SphereCollider sphere;

			Kinematics rayRotations;
			Kinematics sphereKinematics;

			//std::string intersectionTimeString = "no intersection";

			void Init()
			{
				initFresh();
				currCamera.pos = glm::vec3(5.0f, 5.0f, 5.0f);

				changeBackGroundOnCollision = false;
				showAxis = false;
				wireFrameMode = false;

				ray.startPoint = glm::vec3(5.0f, 0.0f, 0.0f);
				ray.length = 10.0f;
				ray.model.color = glm::vec3(0.0f, 1.0f, 1.0f);

				//Rotations are based on right vector transformations
				ray.meshID = MeshNames::rayRight;

				//rotate around y-axis
				rayRotations.normVector = glm::vec3(0.0f, 1.0f, 0.0f);
				rayRotations.speed = 25.0f;

				sphere.centerPos = glm::vec3(0.0f, 0.0f, 0.0f);
				sphere.radius = 2.0f;

				sphere.model.color = basicBlue;

				sphereKinematics.speed = 0.0f;
				sphereKinematics.normVector = worldForward;
			}

			void RenderSettings()
			{
				RenderRayUI(ray, "Ray");
				RenderSphereUI(sphere, "Sphere");
				RenderKinematics(rayRotations, "Ray Rotations");
				RenderKinematics(sphereKinematics, "Sphere Speed");

			}

			void Update()
			{
				RenderSettings();
				UpdatePhysics(ray.model.rotDegrees, rayRotations);
				UpdateRay(ray);

				sphere.UpdateModel();


				if (checkRayOnSphere(ray, sphere))
				{
					//intersectionTimeString = ("intersection time at:" + std::to_string((getIntersectionTimeRayOnPlane(ray, plane))));

					sphere.model.color = greenscreenGreen;
					ray.model.color = glm::vec3(1.0f, 0.0f, 0.0f);
				}
				else
				{
					sphere.model.color = basicBlue;
					ray.model.color = glm::vec3(1.0f, 1.0f, 1.0f);
				}
			}


			void Render()
			{
				RenderAxis();

				SubmitDraw(ray.model, MeshNames::rayRight, colorShader.shaderName);
				SubmitDraw(sphere.model, MeshNames::sphere, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}
		}

		namespace RayVsAABB
		{
			Ray ray;
			AABB box;

			Kinematics rayRotations;
			Kinematics boxKinematics;

			void Init()
			{
				initFresh();
				currCamera.pos = glm::vec3(5.0f, 5.0f, 5.0f);

				changeBackGroundOnCollision = false;
				showAxis = false;
				wireFrameMode = false;

				ray.startPoint = glm::vec3(5.0f, 0.0f, 0.0f);
				ray.length = 10.0f;
				ray.model.color = glm::vec3(0.0f, 1.0f, 1.0f);

				//Rotations are based on right vector transformations
				ray.meshID = MeshNames::rayRight;

				//rotate around y-axis
				rayRotations.normVector = glm::vec3(0.0f, 1.0f, 0.0f);
				rayRotations.speed = 25.0f;

				box.centerPos = glm::vec3(0.0f, 0.0f, 0.0f);
				box.scale = glm::vec3(2.0f, 2.0f, 2.0f);
			}

			void RenderSettings()
			{
				RenderRayUI(ray, "Ray");
				RenderAABBUI(box, "Box");
				RenderKinematics(rayRotations, "Ray Rotations");
				RenderKinematics(boxKinematics, "Box Speed");

			}

			void Update()
			{
				RenderSettings();
				UpdatePhysics(ray.model.rotDegrees, rayRotations);
				UpdateRay(ray);

				box.CalculatePoints();
				box.UpdateModel();


				if (checkRayOnAABB(ray, box))
				{
					//intersectionTimeString = ("intersection time at:" + std::to_string((getIntersectionTimeRayOnPlane(ray, plane))));

					box.model.color = greenscreenGreen;
					ray.model.color = glm::vec3(1.0f, 0.0f, 0.0f);
				}
				else
				{
					box.model.color = basicBlue;
					ray.model.color = glm::vec3(1.0f, 1.0f, 1.0f);
				}
			}


			void Render()
			{
				RenderAxis();

				SubmitDraw(ray.model, MeshNames::rayRight, colorShader.shaderName);
				SubmitDraw(box.model, MeshNames::cube, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}

		}

		namespace PointVsTriangle
		{
			Triangle triangle;
			glm::vec3 point;
			Ray ray;

			Transform pointModel;
			glm::vec3 pointModelScale = glm::vec3(0.01f, 0.01f, 0.01f);

			Kinematics pointPhysics;
			Kinematics rayRotations;

			void Init()
			{
				initFresh();
				showAxis = false;
				currCamera.pos = glm::vec3(1.0f, 1.0f, 1.0f);

				triangle.pt0 = glm::vec3(0.0f, 1.0f, 0.0f);
				triangle.pt1 = glm::vec3(-1.0f, 0.0f, 0.0f);
				triangle.pt2 = glm::vec3(1.0f, 0.0f, 0.0f);

				ray.startPoint = glm::vec3(0.0, 0.0f, 3.0f);
				ray.model.color = glm::vec3(0.0f, 1.0f, 1.0f);

				//Rotations are based on right vector transformations
				ray.meshID = MeshNames::rayRight;

				//rotate around y-axis
				rayRotations.normVector = glm::vec3(0.0f, 1.0f, 0.0f);
				rayRotations.speed = 25.0f;

				ray.length = 5.0f;

				point = glm::vec3(-8.0f, 0.5f, 0.0f);
				pointPhysics.speed = 1.0f;
				pointPhysics.normVector = glm::vec3(1.0f, 0.0f, 0.0f);

			}

			void RenderUI()
			{

				ImGui::DragFloat3("Point", (float*)&point, 0.1f, -10.0f, 10.0f);
				RenderKinematics(pointPhysics, "point speed");

				RenderKinematics(rayRotations, "ray rotations");
				RenderRayUI(ray, "Ray");

				RenderTriangleUI(triangle, "triangle settings");
			}

			void Update()
			{
				pointModel.scale = glm::vec3(0.1f, 0.1f, 0.1f);
				pointModel.pos = point;

				UpdatePhysics(ray.model.rotDegrees, rayRotations);
				UpdatePhysics(point, pointPhysics);
				UpdateRay(ray);

				triangle.rayA.color = glm::vec3(1.0f, 1.0f, 1.0f);
				triangle.rayB.color = glm::vec3(1.0f, 1.0f, 1.0f);
				triangle.rayC.color = glm::vec3(1.0f, 1.0f, 1.0f);

				if (checkPointOnTriangle(triangle, point) || checkRayOnTriangle(triangle, ray))
				{
					collisionDetected = true;
					triangle.rayA.color = glm::vec3(1.0f, 0.0f, 0.0f);
					triangle.rayB.color = glm::vec3(1.0f, 0.0f, 0.0f);
					triangle.rayC.color = glm::vec3(1.0f, 0.0f, 0.0f);
				}

				RenderUI();
				UpdateTriangle(triangle);
			}

			void Render()
			{
				RenderAxis();
				SubmitDraw(pointModel, MeshNames::sphere, colorShader.shaderName);
				SubmitDraw(triangle.rayA, MeshNames::rayRight, colorShader.shaderName);
				SubmitDraw(triangle.rayB, MeshNames::rayRight, colorShader.shaderName);
				SubmitDraw(triangle.rayC, MeshNames::rayRight, colorShader.shaderName);
				SubmitDraw(ray.model, MeshNames::rayForward, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}



		}



		/**
		namespace PlaneVsAABB
		{
			Plane plane;
			glm::vec3 cameraStartPos = glm::vec3(1.0f, 1.0f, 1.0f);
			float rotateSpeed = 10.0f;

			float currDistanceFromPlane = -1.11f;

			AABB box;
			Kinematics boxKinematics;

			//Can use a sphere to represent it
			Transform pointModel;
			glm::vec3 pointCollision = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::vec3 boxStartPos = glm::vec3(0.0f, 1.0f, -5.0f);
			glm::vec3 boxStartScale = glm::vec3(1.0f, 1.0f, 1.0f);
			float boxStartSpeed = 2.0f;
			glm::vec3 boxVelocityNorm = worldForward;



			//For the physical representation. Does not influence collision
			glm::vec3 pointModelScale = glm::vec3(0.01f, 0.01f, 0.01f);

			void Init()
			{
				//Default for this scene is a little different
				changeBackGroundOnCollision = false;
				showAxis = false;
				wireFrameMode = true;

				//Closer for this scene
				topDownCameraHeight = 1.0f;

				box.model.color = greenscreenGreen;
				box.centerPos = boxStartPos;
				box.scale = boxStartScale;

				boxKinematics.speed = boxStartSpeed;
				boxKinematics.normVector = worldForward;

				currCamera.pos = cameraStartPos;

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;

				plane.outwardNormal = worldRight;

				plane.model.scale = glm::vec3(1.0f, 1.0f, 1.0f);
				plane.pointOnPlane = glm::vec3(0.0f, 0.0f, 0.0f);
				plane.normalModel.color = skyBlue;
				plane.normalModel.scale = glm::vec3(1.0f, 1.0f, 10.0f);

			}

			void RenderSettings()
			{
				ImGui::Begin("Plane vs AABB Settings");
				ImGui::Spacing();

				ImGui::Text(("Point nearest distance from plane: " + std::to_string(currDistanceFromPlane)).c_str());
				ImGui::Spacing();

				RenderPlaneUI(plane, "Plane");
				//ImGui::DragFloat("Plane Rotation Speed", (float*)&rotateSpeed, 90.0f, 0.0f, 360.0f);
				ImGui::DragFloat3("Point", (float*)&pointCollision, 0.01f, -100.0f, 100.0f);


				ImGui::End();
			}

			void Update()
			{
				//UpdatePhysics(pointCollision, pointKinematics);


				pointModel.pos = pointCollision;


				currDistanceFromPlane = distanceFromPlane(pointCollision, plane);

				RenderSettings();
				UpdatePlane(plane);


				if (checkPointOnPlane(pointCollision, plane))
				{


					collisionDetected = true;
					plane.model.color = glm::vec3(1.0f, 0.0f, 0.0f); //Red
					//backgroundColor = collidedBackgroundColor;
				}
				else
				{
					plane.model.color = glm::vec3(1.0f, 1.0f, 1.0f);

					//if (backgroundColor == collidedBackgroundColor)
					//	//backgroundColor = neutralBackgroundColor;
				}

			}



			void Render()
			{
				RenderAxis();

				SubmitDraw(plane.model, MeshNames::quadNormalRight, colorShader.shaderName);
				SubmitDraw(plane.normalModel, MeshNames::rayRight, colorShader.shaderName);

				SubmitDraw(pointModel, MeshNames::sphere, colorShader.shaderName);

				DrawAll(drawList, currCamera);
				RenderPictureinPicture();
				drawList.clear();
			}
		}
		  */
	}

	void InitScenes()
	{
		currentSceneName = SceneNames::defaultScene;

		sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::defaultScene,
			AssignmentScene{ Assignment::AssignmentOneScenes::Default::Init,
			Assignment::AssignmentOneScenes::Default::Render,  Assignment::AssignmentOneScenes::Default::Update }));


		//Previous way of adding scenes to the scene container. 
		//Replaced with a more polymorphic approach that is only one function call and therefore less error prone

#ifdef ASSIGNMENT_ONE

		{
			//AssignmentScene cubeScene;
			//cubeScene.initScene = Assignment::AssignmentOneScenes::Cube::Init;
			//cubeScene.renderScene = Assignment::AssignmentOneScenes::Cube::Render;
			//cubeScene.updateScene = Assignment::AssignmentOneScenes::Cube::Update;
			//sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::TestSceneCube, AssignmentScene(cubeScene)));


			AssignmentScene sphereScene;
			sphereScene.initScene = Assignment::AssignmentScenes::Sphere::Init;
			sphereScene.renderScene = Assignment::AssignmentScenes::Sphere::Render;
			sphereScene.updateScene = Assignment::AssignmentScenes::Sphere::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::TestSceneSphere, AssignmentScene(sphereScene)));

			AssignmentScene sphereSceneTwo;
			sphereSceneTwo.initScene = AssignmentScenes::SphereVsSphere::Init;
			sphereSceneTwo.renderScene = AssignmentScenes::SphereVsSphere::Render;
			sphereSceneTwo.updateScene = AssignmentScenes::SphereVsSphere::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::SphereVsSphere, AssignmentScene(sphereSceneTwo)));

			AssignmentScene AABB_vs_Sphere;
			AABB_vs_Sphere.initScene = AssignmentScenes::AABBVsSphere::Init;
			AABB_vs_Sphere.renderScene = AssignmentScenes::AABBVsSphere::Render;
			AABB_vs_Sphere.updateScene = AssignmentScenes::AABBVsSphere::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::AABBVsSphere, AssignmentScene(AABB_vs_Sphere)));

			AssignmentScene SphereVsAABB;
			SphereVsAABB.initScene = AssignmentScenes::SphereVsAABB::Init;
			SphereVsAABB.renderScene = AssignmentScenes::SphereVsAABB::Render;
			SphereVsAABB.updateScene = AssignmentScenes::SphereVsAABB::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::SphereVsAABB, AssignmentScene(SphereVsAABB)));

			AssignmentScene AABBVsAABB;
			AABBVsAABB.initScene = AssignmentScenes::AABBvsAABB::Init;
			AABBVsAABB.renderScene = AssignmentScenes::AABBvsAABB::Render;
			AABBVsAABB.updateScene = AssignmentScenes::AABBvsAABB::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::AABBVsAABB, AssignmentScene(AABBVsAABB)));

			AssignmentScene PointVsAABB;
			PointVsAABB.initScene = AssignmentScenes::PointVsAABB::Init;
			PointVsAABB.renderScene = AssignmentScenes::PointVsAABB::Render;
			PointVsAABB.updateScene = AssignmentScenes::PointVsAABB::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::PointVsAABB, AssignmentScene(PointVsAABB)));

			AssignmentScene PointVsSphere;
			PointVsSphere.initScene = AssignmentScenes::PointVsSphere::Init;
			PointVsSphere.renderScene = AssignmentScenes::PointVsSphere::Render;
			PointVsSphere.updateScene = AssignmentScenes::PointVsSphere::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::PointVsSphere, AssignmentScene(PointVsSphere)));

			AssignmentScene PointVsPlane;
			PointVsPlane.initScene = AssignmentScenes::PointVsPlane::Init;
			PointVsPlane.renderScene = AssignmentScenes::PointVsPlane::Render;
			PointVsPlane.updateScene = AssignmentScenes::PointVsPlane::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::PointVsPlane, AssignmentScene(PointVsPlane)));

			AssignmentScene PlaneVsSphere;
			PlaneVsSphere.initScene = AssignmentScenes::PlaneVsSphere::Init;
			PlaneVsSphere.renderScene = AssignmentScenes::PlaneVsSphere::Render;
			PlaneVsSphere.updateScene = AssignmentScenes::PlaneVsSphere::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::PlaneVsSphere, AssignmentScene(PlaneVsSphere)));

			AssignmentScene RayVsPlane;
			RayVsPlane.initScene = AssignmentScenes::RayVsPlane::Init;
			RayVsPlane.renderScene = AssignmentScenes::RayVsPlane::Render;
			RayVsPlane.updateScene = AssignmentScenes::RayVsPlane::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::RayVsPlane, AssignmentScene(RayVsPlane)));

			AssignmentScene RayVsSphere;
			RayVsSphere.initScene = AssignmentScenes::RayVsSphere::Init;
			RayVsSphere.renderScene = AssignmentScenes::RayVsSphere::Render;
			RayVsSphere.updateScene = AssignmentScenes::RayVsSphere::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::RayVsSphere, AssignmentScene(RayVsSphere)));

			AssignmentScene RayVsAABB;
			RayVsAABB.initScene = AssignmentScenes::RayVsAABB::Init;
			RayVsAABB.renderScene = AssignmentScenes::RayVsAABB::Render;
			RayVsAABB.updateScene = AssignmentScenes::RayVsAABB::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::RayVsAABB, AssignmentScene(RayVsAABB)));


			AssignmentScene PlaneVsAABB;
			PlaneVsAABB.initScene = AssignmentScenes::PlaneVsAABB::Init;
			PlaneVsAABB.renderScene = AssignmentScenes::PlaneVsAABB::Render;
			PlaneVsAABB.updateScene = AssignmentScenes::PlaneVsAABB::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::PlaneVsAABB, AssignmentScene(PlaneVsAABB)));

			AssignmentScene PointVsTriangle;
			PointVsTriangle.initScene = AssignmentScenes::PointVsTriangle::Init;
			PointVsTriangle.renderScene = AssignmentScenes::PointVsTriangle::Render;
			PointVsTriangle.updateScene = AssignmentScenes::PointVsTriangle::Update;
			sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::PointVsTriangle, AssignmentScene(PointVsTriangle)));
		}

#endif // ASSIGNMENT_ONE


		//Only one line. So much better :)
		using namespace AssignmentOneScenes;
		ScenetoFunction<CubeScene>(SceneNames::TestSceneCube);
		ScenetoFunction<AssimapExample>(SceneNames::AssimapExample);

		using namespace AssignmentTwoScenes;
		ScenetoFunction<CubeLoadScene>(SceneNames::CubeSceneTest);

	}
}


//Initialization and Update Loop
namespace Assignment
{



	void UpdateAssignment()
	{
		collisionDetected = false;
		sceneMap.at(currentSceneName).updateScene();
	}

	void RenderAssignment()
	{
		glViewport(0.0f, 0.0f, applicationPtr->getWindowWidth(), applicationPtr->getWindowHeight());

		RenderDearImguiDefault();
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		if (enableDepthTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (wireFrameMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		sceneMap.at(currentSceneName).renderScene();
	}

	int InitAssignment()
	{
		assignmentShaders.loadShader(defaultShader.shaderName, defaultShader);
		assignmentShaders.loadShader(colorShader.shaderName, colorShader);

		std::cout << "Assignment Shaders Loaded\n";

		if (runDebugTests)
		{
			Tests::RunAllTests();
		}

		//topDownCamera.pos = topDownCameraHeight;
		//topDownCamera.right = worldRight;
		//topDownCamera.up = -worldForward;

		InitMeshes();
		InitPictureInPicture();

		Transform model;

		modelMap.insert(std::make_pair<std::string, Transform>(ModelNames::defaultModel, Transform(model)));

		InitScenes();
		currentSceneName = SceneNames::CubeSceneTest;
		SetScene(currentSceneName);

		return 0;
	}
}





