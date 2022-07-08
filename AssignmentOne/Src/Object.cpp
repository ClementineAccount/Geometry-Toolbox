#include "Object.h"

namespace Assignment
{
	std::vector<glm::vec3> GetObjectPositions(Object const& obj)
	{
		std::vector<glm::vec3> positions;

		glm::mat4 modelMat = calculateModel(obj.transform);
		for (auto const& localPos : obj.objectMesh->meshVertices.positions)
		{
			glm::vec4 modelPos = modelMat * glm::vec4(localPos.x, localPos.y, localPos.z, 1.0f);
			positions.push_back(glm::vec3(modelPos.x, modelPos.y, modelPos.z));
		}

		return positions;
	}

	std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList, size_t start, size_t end)
	{
		//maybe can just store pointer to the positions? that way there will be no need allocate extra memory
		std::vector<glm::vec3> positions;

		//The caller will have to pass in (size - 1) if they wish to go until the end
		for (size_t i = start; i <= end; ++i)
		{
			std::vector<glm::vec3> objPositions = GetObjectPositions(objectList[i]);
			positions.insert(positions.end(), objPositions.begin(), objPositions.end());
		}

		return positions;
	}

	std::vector<glm::vec3> GetObjectPositions(std::vector<Object> const& objectList)
	{
		return GetObjectPositions(objectList, 0, objectList.size() - 1);
	}

	std::vector<glm::vec3> GetObjectPositions(std::vector<Object const*> const& objectList)
	{
		std::vector<glm::vec3> pos;
		for (Object const* obj : objectList)
		{
			std::vector<glm::vec3> pos2 = GetObjectPositions(*obj);
			pos.insert(pos.end(), pos2.begin(), pos2.end());
		}
		return pos;
	}


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
