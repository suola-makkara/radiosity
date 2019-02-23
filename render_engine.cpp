#include <iostream>

#include "render_engine.hpp"
#include "scene.hpp"
#include "gui.hpp"
#include "settings.hpp"

void RenderEngine::initialize(GLFWwindow *window)
{
	camera = Camera(window, glm::vec3(0.0f, 0.0f, 10.0f));
	camera.disable(window);

	basicShader = ShaderProgram("basic.vs", "basic.fs");
}

void RenderEngine::render()
{
	basicShader.use();
	basicShader.setMat4fv("view", camera.getViewMat());
	basicShader.setMat4fv("proj", glm::perspective(glm::radians(45.0f),
				1.0f * Settings::windowWidth / Settings::windowHeight,
				0.1f, 100.0f));

	std::list<Plane> &objects = Scene::getObjects();

	for (auto it = objects.begin(); it != objects.end(); it++)
		it->draw(basicShader);

	GUI::render();
}

void RenderEngine::updateCamera(GLFWwindow *window) {
	camera.update(window, 0.05f); } // <-- DT

void RenderEngine::toggleCamera(GLFWwindow *window)
{
	cameraEnabled = !cameraEnabled;

	if (cameraEnabled) camera.enable(window);
	else camera.disable(window);
}

glm::mat4 RenderEngine::getViewMatrix() { return camera.getViewMat(); }

Camera RenderEngine::camera = Camera();
bool RenderEngine::cameraEnabled = false;

ShaderProgram RenderEngine::basicShader = ShaderProgram();
