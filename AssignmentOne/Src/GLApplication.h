/*****************************************************************//**
 * \file   GLApplication.h
 * \brief  Header file for GLApplication for global
 *         helper functions that can be used for quick prototyping
 *         before refactoring into proper classes
 * 
 * \author Clementine Shamaney, clementine.s@digipen.edu
 * \date   May 2022
 *********************************************************************/
#pragma once

//forward declare

class GLFWindow;

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include "SceneManager.h"
#include "Scene.h"


#include <functional>
#include <vector>

namespace GeometryToolbox
{
	class GLApplication
	{
		//Possible to do: encapsulation if necessary of member variables
		public:

			Scenes::SceneManager sm;
			GLFWwindow* window;

			double gDeltaTime;
			double gLastFrameTime;

			//16:9 aspect ratio for testing (To Do: encapsulate this to ensure resizing is working properly
			int gWindowWidth;
			int gWindowHeight;

			//Calculate and cache this if window size change
			GLfloat gAspectRatio;

			std::vector<std::function<void(void)>> updateFunctions;

			//Eventually we'd want to have a SceneManager here or a way to call it so that Update calls it

		public:

			GLApplication() {}; 
			~GLApplication() {};

			/**
			 * Creates the application
			 * 
			 * \return
			 * error codes 
			 */
			int initApplication();

			int updateApplication();

			int shutdownApplication();

			/**
			* Returns a value copy of current deltaTime, which is calculated after every frame
			* and defined as the time between frames.
			*
			*
			* \return
			* a value copy of gDeltaTime
			*/
			double getDeltaTime();

			/**
			 * Returns a value of current window height.
			 */
			int getWindowHeight();

			/**
			 * Returns value of current window width.
			 */
			int getWindowWidth();

			float getAspectRatio();


	};
}

