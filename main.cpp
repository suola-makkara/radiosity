#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <climits>

#include "gui.hpp"
#include "render_engine.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "editor.hpp"
#include "radiosity_render_engine.hpp"

void windowCloseCallback(GLFWwindow *window);
void windowSizeCallback(GLFWwindow *window, int widht, int height);

void cursorPositionCallback(GLFWwindow *window, double posX, double posY);
void mouseButtonCallback(GLFWwindow *window, int button,
		int action, int mods);

void keyCallback(GLFWwindow *window, int key, int scancode,
		int action, int mods);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(Settings::windowWidth,
			Settings::windowHeight, "Radiosity", NULL, NULL);

	glfwSetWindowSizeLimits(window, Settings::minWindowWidth,
			Settings::minWindowHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

	glfwSetWindowCloseCallback(window, windowCloseCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, Settings::windowWidth, Settings::windowHeight);

	RenderEngine::initialize(window);
	GUI::initialize(window);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
				GL_STENCIL_BUFFER_BIT);
		
		if (!RenderEngine::isCameraEnabled() &&
				!GUI::isActive() && Settings::mode == Settings::EDIT)
			Editor::findHoveredObject(window);
		else
			Editor::setHoveredObject(nullptr);

		RenderEngine::updateCamera(window);

		GUI::update(window);

		RenderEngine::render();

		glfwSwapBuffers(window);
	}
    
	GUI::destruct();
	Scene::destruct();
	RadiosityRenderEngine::destruct();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void windowCloseCallback(GLFWwindow *window)
{
	GUI::showQuitWindow = true;
	glfwSetWindowShouldClose(window, GLFW_FALSE);
}

void windowSizeCallback(GLFWwindow *window, int width, int height)
{
	Settings::windowWidth = width;
	Settings::windowHeight = height;
	glViewport(0, 0, width, height);
}

void cursorPositionCallback(GLFWwindow *window,
		double posX, double posY) { }

void mouseButtonCallback(GLFWwindow *window, int button,
		int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS && !GUI::isActive()
				&& Settings::mode == Settings::EDIT)
		{
			Editor::setSelectedObject(Editor::getHoveredObject());
			if (Editor::getHoveredObject() != nullptr)
				GUI::openEditor();
		}
		break;
	}
}

void keyCallback(GLFWwindow *window, int key, int scancode,
		int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_G:
		if (action == GLFW_RELEASE)
		{
			if (RenderEngine::isCameraEnabled())
				RenderEngine::disableCamera(window);
			else if (!GUI::isPopupOpen())
				RenderEngine::enableCamera(window);
		}
		break;
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_RELEASE)
		{
			if (RenderEngine::isCameraEnabled())
				RenderEngine::disableCamera(window);
			else if (Settings::mode == Settings::EDIT)
				Editor::setSelectedObject(nullptr);
		}
		break;
	}
}
