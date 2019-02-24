#ifndef CAMERA
#define CAMERA

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
private:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	float speed = 5.0f;

	float pitch = 0.0f;
	float yaw = 0.0f;

	float mouseSensitivity = 0.15f;
	double prevMousePosX;
	double prevMousePosY;

	bool enabled;
public:
	Camera() = default;
	Camera(GLFWwindow *window,
			const glm::vec3 &position = glm::vec3(0),
			const glm::vec3 &direction = glm::vec3(0.0f, 0.0f, -1.0f),
			const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

	void update(GLFWwindow *window, float dTime);

	glm::mat4 getViewMat();

	glm::vec3 getPos() const { return position; }
	glm::vec3 getDir() const { return direction; }

	void disable(GLFWwindow *window);
	void enable(GLFWwindow *window);

	bool isEnabled() const { return enabled; }
};

#endif // CAMERA
