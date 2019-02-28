#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "shader.hpp"
#include "camera.hpp"

class RenderEngine
{
public:
	static void initialize(GLFWwindow *window);

	static void render();

	static void updateCamera(GLFWwindow *window, float dt);

	static void enableCamera(GLFWwindow *window);
	static void disableCamera(GLFWwindow *window);
	static bool isCameraEnabled();

	static glm::vec3 getCameraPosition();
	static glm::mat4 getViewMatrix();
private:
	static Camera camera;

	static ShaderProgram basicShader;
	static ShaderProgram radiosityShader;
};

#endif // RENDER_ENGINE
