#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "plane.hpp"

class Editor
{
public:
	static void findHoveredObject(GLFWwindow *window);
	
	static void reset();

	static SubdividedPlane* getHoveredObject();
	static void setHoveredObject(SubdividedPlane *object);

	static SubdividedPlane* getSelectedObject();
	static void setSelectedObject(SubdividedPlane *object);
	
	static bool viewSubdivisions;
private:
	static SubdividedPlane* hoveredObject;
	static SubdividedPlane* selectedObject;
};

#endif // EDITOR_HPP
