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

namespace GLApplication
{
	
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
}
