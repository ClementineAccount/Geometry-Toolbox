/*****************************************************************//**
 * \file   Global.hpp
 * \brief  Various global helper functions and global variables for easy and quick 
 *         debugging and prototyping
 *         Some of these may be refactored into proper classes
 * 
 * \author Clementine Shamaney, clementine.s@digipen.edu
 * \date   May 2022
 *********************************************************************/
#pragma once

#include <GLFW/glfw3.h>

namespace global
{
	double lastFrameTime;
	double deltaTime;

	/**
	 * Updates the deltaTime value using GLFW
	 * deltaTime refers to the time in seconds between frames.
	 * 
	 */
	void UpdateDeltaTime()
	{
		double currentFrame = glfwGetTime();
		lastFrameTime = currentFrame;
		deltaTime = currentFrame - lastFrameTime;
	}
}
