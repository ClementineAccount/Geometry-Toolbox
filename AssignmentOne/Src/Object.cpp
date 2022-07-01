#include "Object.h"

namespace Assignment
{
	glm::mat4 calculateModel(Transform currModel)
	{
		auto makePivotVector = [](Transform const& model)
		{
			return glm::vec3(
				model.pivotPercent.x * model.scale.x,
				model.pivotPercent.y * model.scale.y,
				model.pivotPercent.z * model.scale.z);
		};

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

		return modelMat;
	}
}
