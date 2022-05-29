#pragma once
#include "AssignmentOne.h"

#include <memory>

namespace AssignmentOne
{
	struct AABB
	{

		Model model; //the transforms
		std::string meshID; //To Do: You could change this to use uint_32 instead eventually
		bool isActive = true;
	};



	class CollisionHelper
	{

	};
}



