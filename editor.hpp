#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "plane.hpp"

class Editor
{
public:
	static void findHoveredObject(GLFWwindow *window);
	
	static void reset();

	static Plane* getHoveredObject();
	static void setHoveredObject(Plane *object);

	static Plane* getSelectedObject();
	static void setSelectedObject(Plane *object);
private:
	static Plane* hoveredObject;
	static Plane* selectedObject;
};

#endif // EDITOR_HPP
