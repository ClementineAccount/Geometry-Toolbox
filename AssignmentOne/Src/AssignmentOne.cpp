#include "AssignmentOne.h"
#include "AssignmentOneSettings.h"

#include "CollisionHelper.h"

#include <functional>
#include <iostream>

#include <map>

// Include Dear Imgui
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


//Settings
namespace AssignmentOne
{
	const std::string shaderFolderPath = "../AssignmentOne/Shaders/";

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



	//'Scenes' that are just a collection of functions. Took away some abstraction from a more overengineered implementation
	namespace SceneNames
	{
		const char defaultScene[] = "defaultScene";

		//Just the axis and cube
		const char TestSceneCube[] = "Cube Example";

		//Just the axis and sphere
		const char TestSceneSphere[] = "Sphere Example";

		const char SphereOnSphere[] = "Sphere on Sphere";
	}


	std::string currentSceneName;


	//To Do: Can allow dynamic model names here too, of course
	namespace ModelNames
	{
		const char defaultModel[] = "default";
		const char floorPlane[] = "floorPlaneModel";
		const char pointModel[] = "pointModel"; //for debugging posBuffer
	}

	constexpr bool runDebugTests = true;
}




namespace AssignmentOne
{
	
	std::vector<GLfloat> upQuadPositions =
	{
		-1.0f, 0.0f,  1.0f,  // 0
		 1.0f, 0.0f,  1.0f,  // 1
		 1.0f, 0.0f, -1.0f,  // 2
		 1.0f, 0.0f, -1.0f,  // 3
		-1.0f, 0.0f,  1.0f, // 4
		-1.0f, 0.0f, -1.0f  // 5
	};

	std::map<std::string, AssignmentScene> sceneMap;

	std::unordered_map<std::string, Model> modelMap;

	Mesh quadMesh;
	ShaderContainer assignmentShaders;

	//Default camera
	Camera defaultLookAtCamera;
	Camera topDownCamera = defaultLookAtCamera;
	Camera currCamera = defaultLookAtCamera;
	//Camera firstPersonCamera;


	ViewportModel topRightViewport;
	ViewportModel topRightViewportBorder; //for a border effect

	//Yea its ugly but just set it in the main. I'll abstract it away later
	GeometryToolbox::GLApplication* applicationPtr;

	void setApplicationPtr(GeometryToolbox::GLApplication& appPtr)
	{
		applicationPtr = &appPtr;
	}

	static std::vector<drawCall> drawList;


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
	Mesh initVBO(std::vector<GLfloat> meshPositions, std::vector<GLfloat> meshColor)
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
	Mesh InitAxis(glm::vec3 axisRight = worldRight, glm::vec3 axisUp = worldUp, glm::vec3 axisForward = worldForward)
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

		Mesh axisMesh = initVBO(axisPoints, axisColor);
		axisMesh.drawType = GL_LINES;
		return axisMesh;
	}


	//Line that is affected by MVP projection (so it exists in the world). The default orientation is pointing to the Right vector
	Mesh InitWorldLine(glm::vec3 basisVector = unitLineBasisVector, float lineScale = unitLineScale)
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

		Mesh axisMesh = initVBO(linePoints, lineColor);
		axisMesh.drawType = GL_LINES;
		return axisMesh;
	}

	//Unit 1x1x1 Cube for Model transform
	//Mesh InitCubeMesh(float cubeScale = 0.5f)
	//{
	//	return Mesh mesh;
	//}

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

		return initVBO(finalBuffer, colorBuffer);
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

		Mesh pointMesh = initVBO(point, color);
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
	Mesh initSphereMesh(glm::vec3 sphereColor = defaultSphereColor, unsigned int numSlices = 36, unsigned int numStacks = 18, float sphereRadius = 1.0f)
	{
		float PI = 3.14f;

		float sliceStep = 2 * PI / numSlices;
		float stackStep = PI / numStacks;
		float sliceAngle, stackAngle;

		float x, y, z, stack_xy;
		std::vector<GLfloat> positions;

		for (size_t currStack = 0; currStack <= numStacks; ++currStack)
		{
			//starting from the bottom stack (so we rotate ccw by 270 and then add onto that)
			stackAngle = PI / 2 - currStack * stackStep;

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
		Mesh sphereMesh = initVBO(positions, colorBuffer);


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


	//Didn't finish it 
	//Ring for testing. It's the same as a stack for the Sphere (ring is similar to sphere so it uses 0, 0, 0 as center
	Mesh initRingMesh(glm::vec3 ringColor = defaultRingColor, unsigned int numSlices = 4, float heightAngleDegree = 30.0f, float ringRadiusScale = 1.0f)
	{
		std::vector<GLfloat> posBuffer;

		//glm::vec3 radiusRightVector = worldRight * ringRadiusScale;

		////Each slice has a 'pair' for the stack
		//for (size_t currSlice = 0; currSlice <= numSlices; ++currSlice)
		//{
		//	//Stacks must alternative
		//	float thisPhi = MeshConstructor::Sphere::getPhiDegrees(currSlice, numSlices);
		//	float stackOne = heightAngleDegree;
		//	float stackTwo = heightAngleDegree + heightAngleDegree;

		//	//Rotate about the z-axis
		//	glm::vec4 vectorFromCenter  = glm::vec4(radiusRightVector, 0.0f); //Vector

		//	//about the z-axis
		//	//glm::mat4 rotateMatrix = glm::mat4{glm::vec3()}

		//	vectorFromCenter =  rotateMatrix * vectorFromCenter;

		//	//about the y-axis
		//	rotateMatrix = glm::rotate(thisPhi, worldUp);
		//	vectorFromCenter = rotateMatrix * vectorFromCenter;


		//	posBuffer.push_back(vectorFromCenter.x);
		//	posBuffer.push_back(vectorFromCenter.y);
		//	posBuffer.push_back(vectorFromCenter.z);

		//	//Rotate about the z-axis
		//	vectorFromCenter = glm::vec4(radiusRightVector, 0.0f); //Vector

		//	//about the z-axis
		//	rotateMatrix = glm::rotate(stackTwo, worldForward);
		//	vectorFromCenter = rotateMatrix * vectorFromCenter;


		//	//about the y-axis
		//	
		//	rotateMatrix = glm::rotate(thisPhi, worldUp);
		//	vectorFromCenter = rotateMatrix * vectorFromCenter;

		//	posBuffer.push_back(vectorFromCenter.x);
		//	posBuffer.push_back(vectorFromCenter.y);
		//	posBuffer.push_back(vectorFromCenter.z);
		//}

		std::vector<GLfloat> colorBuffer;

		for (size_t i = 0; i < posBuffer.size() / 3; ++i)
		{
			colorBuffer.push_back(ringColor.r);
			colorBuffer.push_back(ringColor.g);
			colorBuffer.push_back(ringColor.b);
		}

		Mesh ringMesh = initVBO(posBuffer, colorBuffer);
		ringMesh.drawType = GL_TRIANGLE_STRIP;
		return ringMesh;
	}
	


	Mesh initCubeMesh(glm::vec3 cubeColor = defaultCubeColor, float cubeScale = 0.5f)
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

		Mesh cubeMesh = initVBO(posBuffer, colorBuffer);
		cubeMesh.drawType = GL_TRIANGLES;
		return cubeMesh;
	}

	Mesh InitQuadMesh(std::vector<GLfloat>& quadPositions, float quadScale)
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

		Mesh quadMesh = initVBO(quadPositions, quadColors);
		return quadMesh;
	}

	void SubmitDraw(std::string const modelName, std::string const meshName, std::string const shaderName = defaultShader.shaderName)
	{
		drawList.emplace_back(drawCall{ modelMap.at(modelName), meshMap.at(meshName), assignmentShaders.getShaderID(shaderName) });
	}

	void SubmitDraw(Model model, std::string const meshName, std::string const shaderName = defaultShader.shaderName)
	{
		drawList.emplace_back(drawCall{ model, meshMap.at(meshName), assignmentShaders.getShaderID(shaderName)});
	}

	//Fills the entire screen with a quad (example of usage to create a border for Top Left picture-in-view)
	void FillScreen(glm::vec3 colorFill = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		Mesh quadMesh = meshMap.at(MeshNames::quadForward);

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


	void DrawAll(std::vector<drawCall> const& drawList, Camera const& drawCamera)
	{
		auto makePivotVector = [](Model const& model)
		{
			return glm::vec3(
				model.pivotPercent.x * model.scale.x,
				model.pivotPercent.y * model.scale.y,
				model.pivotPercent.z * model.scale.z);
		};

		

		for (drawCall const& currDraw : drawList)
		{
			//Redo the concept of 'isRendering' to avoid iterating through non rendering objects (remove them from the list is better)
			//if (!currDraw.isRendering)
			//	continue;

			Model const& currModel = currDraw.model;

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

			Mesh const& currMesh = currDraw.mesh;

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

		meshMap.emplace(MeshNames::quadForward, InitQuadMesh(quadPlanepos));

		meshMap.emplace(MeshNames::axis, InitAxis());
		meshMap.emplace(MeshNames::axisInverted, InitAxis(-worldRight, -worldUp, -worldForward));
		meshMap.emplace(MeshNames::cube, initCubeMesh());

		meshMap.emplace(MeshNames::worldLine, InitWorldLine());
		meshMap.emplace(MeshNames::sphere, initSphereMesh());
		meshMap.emplace(MeshNames::ring, initRingMesh());

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
		//That way there is a default background for every scene
		backgroundColor = neutralBackgroundColor;

		sceneMap.at(setScene).initScene();
		currentSceneName = setScene;
	}

	//Has to be different per thing later
	void RenderDearImguiDefault()
	{
		ImGui::Begin("Settings");

		ImGui::DragFloat3("Camera Position", (float*) &currCamera.pos, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat3("Camera Target Position", (float*)&currCamera.targetPos, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat("FOV", (float*)&currCamera.FOV, 0.01f, 1.0f, 110.0f);
		ImGui::DragFloat3("Background Color", (float*)&backgroundColor, 0.001f, 0.0f, 1.0f);


		ImGui::Checkbox("Wireframe Mode", &wireFrameMode);

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


		//I have it so PictureInPicture doesn't have wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Treat the viewports like transformations.

		glViewport(topRightViewportBorder.bottomLeft.x, topRightViewportBorder.bottomLeft.y, topRightViewportBorder.viewportScale.x, topRightViewportBorder.viewportScale.y);
		FillScreen();


		glViewport(topRightViewport.bottomLeft.x, topRightViewport.bottomLeft.y, topRightViewport.viewportScale.x, topRightViewport.viewportScale.y);
		
		FillScreen(backgroundColor);

		DrawAll(drawList, topDownCamera);
	}

	void RenderAxis()
	{
		SubmitDraw(ModelNames::defaultModel, MeshNames::axisInverted);
		SubmitDraw(ModelNames::defaultModel, MeshNames::axis);

	}

	void UpdatePhysics(glm::vec3& pos, Kinematics const& kinematics)
	{
		//Currently assumes constant acceleration (also instanaous velocity anyways)
		pos +=  static_cast<GLfloat>(applicationPtr->getDeltaTime()) * (kinematics.speed * kinematics.normVector);
	}

	void RenderAssignmentTesting()
	{

		Model testModel;
		testModel.scale = glm::vec3(0.5f, 0.5f, 0.5f);

		Model sphereModel;
		sphereModel.scale = glm::vec3(10.0f, 10.0f, 10.0f);

		//SubmitDraw(Model{}, meshMap.at(MeshNames::sphere));

		RenderAxis();
		//SubmitDraw(ModelNames::defaultModel, MeshNames::axisInverted);
		//SubmitDraw(ModelNames::defaultModel, MeshNames::axis);


		static Model worldLineTestMode;
		worldLineTestMode.scale = glm::vec3(10.0f, 10.0f, 10.0f);
		worldLineTestMode.pos = glm::vec3(0.0f, 0.5f, 0.0f);
		worldLineTestMode.rotDegrees.y += applicationPtr->getDeltaTime() * 100.0f;

		SubmitDraw(worldLineTestMode, MeshNames::worldLine);

		static Model testCube;
		testCube.scale = glm::vec3(1.0f, 1.0f, 1.0f);
		testCube.rotDegrees.y += applicationPtr->getDeltaTime() * 10.0f;


		DrawAll(drawList, currCamera);

	}

	void UpdateAssignment()
	{
		sceneMap.at(currentSceneName).updateScene();
	}



	void RenderAssignment()
	{
		glViewport(0.0f, 0.0f, applicationPtr->getWindowWidth(), applicationPtr->getWindowHeight());

		RenderDearImguiDefault();
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (wireFrameMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}



		sceneMap.at(currentSceneName).renderScene();
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

		//Testing functions to help with sphere construction
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
				glm::vec2(120.0f +270.0f, 0.0f),
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

	}

}



//Scenes
namespace AssignmentOne
{
	namespace AssignmentScenes
	{
		//AssignmentScenes that does nothing
		namespace Default
		{
			void Init() {};
			void Update() {};
			void Render() {};
		}

		namespace Cube
		{
			Model cubeModel;

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
			Model sphereModel;

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

		namespace SphereCollision 
		{
			//GameObject sphereObject;
				
			SphereCollider sphereOne;
			SphereCollider sphereTwo;
			Kinematics sphereOnePhysics;

			//SphereCollider sphereTwo;

			void Init()
			{
				sphereOne.radius = 0.25f;
				sphereOne.centerPos = glm::vec3(0.0f, 1.0f, -10.0f);
				sphereOnePhysics.speed = 2.0f;
				sphereOnePhysics.normVector = worldForward;
				sphereOne.model.color = basicBlue;

				sphereTwo.radius = 1.0f;
				sphereTwo.centerPos = glm::vec3(0.0f, 1.0f, 0.0f);
				sphereTwo.model.color = greenscreenGreen;

				currCamera.pos = glm::vec3(5.0f, 5.0f, 3.0f);

				backgroundColor = neutralBackgroundColor;
				prevBackGround = backgroundColor;
			}

			void RenderSettings()
			{
				ImGui::Begin("Sphere vs Sphere Settings");

				ImGui::Text("Sphere One");
				ImGui::DragFloat3("pos 1", (float*)&sphereOne.centerPos, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat("radius 1", (float*)&sphereOne.radius, 0.01f, 0.1f, 10.0f);
				ImGui::DragFloat("Sphere One Speed: ", (float*)&sphereOnePhysics.speed, 0.01f, -10.0f, 10.0f);

				ImGui::Text("Sphere Two");
				ImGui::DragFloat3("pos 2", (float*)&sphereTwo.centerPos, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat("radius 2", (float*)&sphereTwo.radius, 0.01f, 0.1f, 10.0f);

				ImGui::End();
			}

			
			void Update()
			{
				RenderSettings();

				UpdatePhysics(sphereOne.centerPos, sphereOnePhysics);

				if (collisionCheck(sphereOne, sphereTwo))
				{
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

	void InitScenes()
	{
		currentSceneName = SceneNames::defaultScene;

		//AssignmentScene defaultScene;
		//defaultScene.initScene = AssignmentOne::AssignmentScenes::Default::Init;
		//defaultScene.renderScene = AssignmentOne::AssignmentScenes::Default::Render;
		//defaultScene.updateScene = AssignmentOne::AssignmentScenes::Default::Update;

		sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::defaultScene,
			AssignmentScene{ AssignmentOne::AssignmentScenes::Default::Init,
			AssignmentOne::AssignmentScenes::Default::Render,  AssignmentOne::AssignmentScenes::Default::Update }));

		AssignmentScene cubeScene;
		cubeScene.initScene = AssignmentOne::AssignmentScenes::Cube::Init;
		cubeScene.renderScene = AssignmentOne::AssignmentScenes::Cube::Render;
		cubeScene.updateScene = AssignmentOne::AssignmentScenes::Cube::Update;
		sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::TestSceneCube, AssignmentScene(cubeScene)));

		AssignmentScene sphereScene;
		sphereScene.initScene = AssignmentOne::AssignmentScenes::Sphere::Init;
		sphereScene.renderScene = AssignmentOne::AssignmentScenes::Sphere::Render;
		sphereScene.updateScene = AssignmentOne::AssignmentScenes::Sphere::Update;
		sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::TestSceneSphere, AssignmentScene(sphereScene)));

		AssignmentScene sphereSceneTwo;
		sphereSceneTwo.initScene = AssignmentScenes::SphereCollision::Init;
		sphereSceneTwo.renderScene = AssignmentScenes::SphereCollision::Render;
		sphereSceneTwo.updateScene = AssignmentScenes::SphereCollision::Update;
		sceneMap.insert(std::make_pair<std::string, AssignmentScene>(SceneNames::SphereOnSphere, AssignmentScene(sphereSceneTwo)));

		//Init every scene one by one 
		std::map<std::string, AssignmentScene>::iterator it;
		for (it = sceneMap.begin(); it != sceneMap.end(); it++)
		{
			it->second.initScene();
		}
	}

	int InitAssignment()
	{
		assignmentShaders.loadShader(defaultShader.shaderName, defaultShader);
		assignmentShaders.loadShader(colorShader.shaderName, colorShader);

		std::cout << "Assignment Shaders Loaded\n";

		if (runDebugTests)
		{
			DebugTesting::TestQuadVertices1();
			DebugTesting::TestQuadVertices2();
			DebugTesting::TestQuadVertices3();
			DebugTesting::TestQuadVertices4();
			DebugTesting::TestSphereAngle1();
			DebugTesting::TestSphereAngle2();
			DebugTesting::TestSphereAngle3();
		}

		//topDownCamera.pos = topDownCameraHeight;
		//topDownCamera.right = worldRight;
		//topDownCamera.up = -worldForward;

		InitMeshes();
		InitPictureInPicture();


		ObjectMaker::MakeFloor();

		Model model;

		modelMap.insert(std::make_pair<std::string, Model>(ModelNames::defaultModel, Model(model)));

		InitScenes();
		currentSceneName = SceneNames::SphereOnSphere;

		return 0;
	}
}


