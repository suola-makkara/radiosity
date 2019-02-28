#include <iostream>

#include "render_engine.hpp"
#include "scene.hpp"
#include "gui.hpp"
#include "settings.hpp"
#include "editor.hpp"
#include "radiosity_render_engine.hpp"

void RenderEngine::initialize(GLFWwindow *window)
{
	camera = Camera(window, glm::vec3(0.0f, 0.0f, 10.0f));
	camera.disable(window);

	basicShader = ShaderProgram("basic.vs", "basic.fs");
	radiosityShader = ShaderProgram("radiosity.vs", "radiosity.fs");
}

void RenderEngine::render()
{
	glm::mat4 viewMat = camera.getViewMat();
	glm::mat4 projMat = glm::perspective(glm::radians(45.0f),
				1.0f * Settings::windowWidth / Settings::windowHeight,
				0.1f, 100.0f);

	if (Settings::mode == Settings::EDIT)
	{
		basicShader.use();
		basicShader.setMat4fv("view", viewMat);
		basicShader.setMat4fv("proj", projMat);

		std::list<SubdividedPlane> &objects = Scene::getObjects();
		
		SubdividedPlane *hoveredObject = Editor::getHoveredObject();
		SubdividedPlane *selectedObject = Editor::getSelectedObject();

		for (auto it = objects.begin(); it != objects.end(); it++)
		{
			if (Editor::viewSubdivisions)
				it->draw(basicShader);
			else
				it->Plane::draw(basicShader);
		}

		glDisable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);

		for (int i = 0; i < 2; i++)
		{
			SubdividedPlane *objectPtr = i == 0 ? hoveredObject :
				selectedObject;

			if (objectPtr == nullptr) continue;

			SubdividedPlane object(*objectPtr);

			// Create stencil for outline
			object.setSize(object.getSize() - 0.1f);

			glStencilFunc(GL_NEVER, 1, 0xFF);

			glStencilFuncSeparate(GL_BACK, GL_NEVER, 1, 0xFF);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		
			object.Plane::draw(basicShader);

			// Draw outline
			object.setSize(object.getSize() + 0.1f);
			object.setColor(i == 0 ?
					glm::vec3(0.3f, 0.3f, 0.8f) :
					glm::vec3(0.3f, 0.8f, 0.3f));

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilOp(GL_ZERO, GL_INCR, GL_INCR);
				
			glDisable(GL_DEPTH_TEST);
			object.Plane::draw(basicShader);
			glEnable(GL_DEPTH_TEST);
		}

		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
	}
	else if (RadiosityRenderEngine::isFinished())
	{
		radiosityShader.use();
		radiosityShader.setMat4fv("view", viewMat);
		radiosityShader.setMat4fv("proj", projMat);

		RadiosityRenderEngine::render(radiosityShader);
	}

	GUI::render();
}

void RenderEngine::updateCamera(GLFWwindow *window) {
	camera.update(window, 0.05f); } // <-- DT

void RenderEngine::enableCamera(GLFWwindow *window)
{
	camera.enable(window);
	GUI::disable();
}

void RenderEngine::disableCamera(GLFWwindow *window)
{
	camera.disable(window);
	GUI::enable();
}

bool RenderEngine::isCameraEnabled() { return camera.isEnabled(); }

glm::vec3 RenderEngine::getCameraPosition() {
	return camera.getPos(); };

glm::mat4 RenderEngine::getViewMatrix() { return camera.getViewMat(); }

Camera RenderEngine::camera = Camera();

ShaderProgram RenderEngine::basicShader = ShaderProgram();
ShaderProgram RenderEngine::radiosityShader = ShaderProgram();
