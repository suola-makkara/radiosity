#ifndef RENDER_ENGINE_HPP
#define RENDER_ENGINE_HPP

#include "shader.hpp"
#include "camera.hpp"

class RenderEngine
{
public:
	static void initialize(GLFWwindow *window);

	static void render();

	static void updateCamera(GLFWwindow *window);
	static void toggleCamera(GLFWwindow *window);

	static glm::vec3 getCameraPosition();
	static glm::mat4 getViewMatrix();

	static bool isCameraEnabled();

	// static void calculateRadiosity();
	// static Progress getRadiosityProgress();
	// static void renderRadiosity(ShaderProgram &shaderProgram);
private:
	static Camera camera;
	static bool cameraEnabled;

	static ShaderProgram basicShader;

	// static std::vector<RadiosityPlane> radiosityObjects;
};

#endif // RENDER_ENGINE
