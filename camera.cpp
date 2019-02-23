#include "camera.hpp"


Camera::Camera(GLFWwindow *window, const glm::vec3 &position,
		const glm::vec3 &direction, const glm::vec3 &up)
	: position(position), direction(direction), up(up), enabled(true)
{
	enable(window);
}

void Camera::update(GLFWwindow *window, float dTime)
{
	if (!enabled) return;

	// Update direction
	double mousePosX, mousePosY;
	glfwGetCursorPos(window, &mousePosX, &mousePosY);

	float dx, dy;
	dx = mousePosX - prevMousePosX;
	dy = mousePosY - prevMousePosY;
	
	prevMousePosX = mousePosX;
	prevMousePosY = mousePosY;

	yaw += dx * mouseSensitivity;
	pitch -= dy * mouseSensitivity;

	if (pitch > 89.0f) pitch = 89.0f;
	else if (pitch < -89.0f) pitch = -89.0f;

	direction.y = sin(glm::radians(pitch));
	direction.x = cos(glm::radians(pitch));

	direction.z = -direction.x * cos(glm::radians(yaw));
	direction.x *= sin(glm::radians(yaw));

	// Update position
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += glm::cross(up,
				glm::normalize(glm::cross(direction, up))) * speed * dTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= glm::cross(up,
				glm::normalize(glm::cross(direction, up))) * speed * dTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position += glm::normalize(glm::cross(up, direction)) * speed * dTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position -= glm::normalize(glm::cross(up, direction)) * speed * dTime;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += up * speed * dTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= up * speed * dTime;
}

glm::mat4 Camera::getViewMat()
{
	glm::vec3 right = glm::normalize(glm::cross(direction, up));
	glm::vec3 cUp = glm::cross(right, direction);
	return glm::translate((glm::mat4)glm::mat3(
				right.x, cUp.x, -direction.x,
				right.y, cUp.y, -direction.y,
				right.z, cUp.z, -direction.z
				), -position);
}

void Camera::disable(GLFWwindow *window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	enabled = false;
}

void Camera::enable(GLFWwindow *window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(window, &prevMousePosX, &prevMousePosY);
	enabled = true;
}
