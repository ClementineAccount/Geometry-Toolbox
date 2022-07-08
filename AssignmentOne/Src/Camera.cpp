#include "Camera.h"

namespace Assignment
{
	//https://learnopengl.com/Getting-started/Camera
	//Also similar to my Vulkan Assignment camera
	void Camera::updateCamera(GeometryToolbox::GLApplication const* app)
	{
		//glfwSetInputMode(app->window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
		int state = glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_RIGHT);
		if (state == GLFW_RELEASE)
		{
			//lastX = app->gWindowWidth / 2.0f;
			//lastY = app->gWindowHeight / 2.0f;
			firstMouse = true;
			updateCameraVectors();
			return;
		}
		
		double xpos, ypos;
		glfwGetCursorPos(app->window, &xpos, &ypos);

		//To Do: Add the check for if the right mouse button is held down
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		offsetX = xpos - lastX;
		offsetY = lastY - ypos;

		offsetX *= sens;
		offsetY *= sens;

		yaw += offsetX;
		pitch += offsetY;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		updateCameraVectors();

	}

	void Camera ::updateCameraMovement(GeometryToolbox::GLApplication const* app)
	{

		float cameraSpeedFinal = cameraSpeed * app->gDeltaTime;

		if (glfwGetKey(app->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			cameraSpeedFinal *= 4.0f;
		}

		if (glfwGetKey(app->window, GLFW_KEY_W) == GLFW_PRESS)
		{
			pos += cameraSpeedFinal * forward;
		}

		if (glfwGetKey(app->window, GLFW_KEY_S) == GLFW_PRESS)
			pos -= cameraSpeedFinal * forward;
		if (glfwGetKey(app->window, GLFW_KEY_A) == GLFW_PRESS)
			pos -= glm::normalize(glm::cross(forward, up)) * cameraSpeedFinal;
		if (glfwGetKey(app->window, GLFW_KEY_D) == GLFW_PRESS)
			pos += glm::normalize(glm::cross(forward, up)) * cameraSpeedFinal;

		if (glfwGetKey(app->window, GLFW_KEY_Q) == GLFW_PRESS)
			pos += glm::normalize(glm::cross(forward, right)) * cameraSpeedFinal;
		if (glfwGetKey(app->window, GLFW_KEY_E) == GLFW_PRESS)
			pos -= glm::normalize(glm::cross(forward, right)) * cameraSpeedFinal;

	}

	void Camera::updateCameraVectors()
	{
		forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward.y = sin(glm::radians(pitch));
		forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		forward = glm::normalize(forward);
		// also re-calculate the Right and Up vector
		right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up = glm::normalize(glm::cross(right, forward));
		targetVector = pos + forward;
	}
}
