#include "editor.hpp"

#include "render_engine.hpp"

void Editor::findHoveredObject()
{
	// RenderEngine::getViewMatrix(); ...
}

Plane* Editor::getHoveredObject() { return hoveredObject; }

Plane* Editor::getSelectedObject() { return selectedObject; }

void Editor::setSelectedObject(Plane *object) { selectedObject = object; }

Plane* Editor::hoveredObject = nullptr;
Plane* Editor::selectedObject = nullptr;
