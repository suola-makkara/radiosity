#include <cmath>
#include <iostream>

#include "editor.hpp"
#include "render_engine.hpp"
#include "settings.hpp"
#include "scene.hpp"

void Editor::findHoveredObject(GLFWwindow *window)
{
	glm::mat4 viewMat = RenderEngine::getViewMatrix();
	float h = Settings::windowHeight /
		(2.0f * std::tan(glm::radians(45.0f / 2.0f)));

	double xPos;
	double yPos;

	glfwGetCursorPos(window, &xPos, &yPos);

	Ray ray;

	ray.origin = RenderEngine::getCameraPosition();
	ray.direction = glm::transpose(glm::mat3(viewMat)) *
		glm::normalize(glm::vec3(xPos - 0.5f * Settings::windowWidth,
					0.5f * Settings::windowHeight - yPos, -h));
	
	std::list<SubdividedPlane>& objects = Scene::getObjects();
	
	float u, v, t, minT = -1.0f;
	for (auto it = objects.begin(); it != objects.end(); it++)
		if (it->intersectsRay(ray, u, v, t))
			if (minT == -1.0f || t < minT)
			{
				minT = t;
				hoveredObject = &(*it);
			}

	if (minT == -1.0f) hoveredObject = nullptr;
}

void Editor::reset() { hoveredObject = nullptr, selectedObject = nullptr; }

SubdividedPlane* Editor::getHoveredObject() { return hoveredObject; }
void Editor::setHoveredObject(SubdividedPlane *object) {
	hoveredObject = object; };

SubdividedPlane* Editor::getSelectedObject() { return selectedObject; }
void Editor::setSelectedObject(SubdividedPlane *object) {
	selectedObject = object; }

bool Editor::viewSubdivisions = false;

SubdividedPlane* Editor::hoveredObject = nullptr;
SubdividedPlane* Editor::selectedObject = nullptr;

