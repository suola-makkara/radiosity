#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <climits>

#include "gui.hpp"
#include "render_engine.hpp"
#include "scene.hpp"
#include "settings.hpp"

void windowCloseCallback(GLFWwindow *window);
void windowSizeCallback(GLFWwindow *window, int widht, int height);
void keyCallback(GLFWwindow *window, int key, int scancode,
		int action, int mods);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(1200, 800, "Radiosity",
			NULL, NULL);

	glfwSetWindowCloseCallback(window, windowCloseCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, 1200, 800);

	RenderEngine::initialize(window);
	GUI::initialize(window);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderEngine::updateCamera(window);

		GUI::update(window);

		RenderEngine::render();

		glfwSwapBuffers(window);
	}
    
	GUI::destruct();
	Scene::destruct();

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

void keyCallback(GLFWwindow *window, int key, int scancode,
		int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_G:
		if (action == GLFW_RELEASE)
			RenderEngine::toggleCamera(window);
		break;
	}
}
