#include <iostream>

#include "render_engine.hpp"
#include "scene.hpp"
#include "gui.hpp"
#include "settings.hpp"
#include "editor.hpp"

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

	Plane *hoveredObject = Editor::getHoveredObject();
	Plane *selectedObject = Editor::getSelectedObject();
	
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		if (&(*it) == hoveredObject || &(*it) == selectedObject) continue;
		it->draw(basicShader);
	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);

	for (int i = 0; i < 2; i++)
	{
		Plane *objectPtr = i == 0 ? hoveredObject : selectedObject;

		if (objectPtr == nullptr) continue;

		Plane object(*objectPtr);

		// Draw object to be outlined
		object.setSize(object.getSize() - 0.1f);

		glStencilFuncSeparate(GL_FRONT, GL_NOTEQUAL, 1, 0xFF);
		glStencilFuncSeparate(GL_BACK, GL_NEVER, 1, 0xFF);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	
		object.draw(basicShader);

		// Draw outline
		object.setSize(object.getSize() + 0.1f);
		object.setColor(i == 0 ?
				glm::vec3(0.3f, 0.3f, 0.8f) :
				glm::vec3(0.3f, 0.8f, 0.3f));

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_ZERO, GL_INCR, GL_INCR);
		
		glDisable(GL_DEPTH_TEST);
		object.draw(basicShader);
		glEnable(GL_DEPTH_TEST);
	}

	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	
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

glm::vec3 RenderEngine::getCameraPosition() {
	return camera.getPos(); };

glm::mat4 RenderEngine::getViewMatrix() { return camera.getViewMat(); }

bool RenderEngine::isCameraEnabled() { return camera.isEnabled(); }

Camera RenderEngine::camera = Camera();
bool RenderEngine::cameraEnabled = false;

ShaderProgram RenderEngine::basicShader = ShaderProgram();
