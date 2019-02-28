#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <string>

#include "plane.hpp"

class Scene
{
public:
	static void loadNew();

	static void load(const std::string &file);

	static void save();

	static void saveAs(const std::string &file);

	static bool hasFileAddress();

	static bool isSaved();

	static void update();

	static std::list<SubdividedPlane>& getObjects();

	static void addObject(const SubdividedPlane &plane);

	static void removeObject(SubdividedPlane *plane);

	static void destruct();
private:
	static std::string file;
	
	static bool saved;

	static std::list<SubdividedPlane> objects;
};

#endif // SCENE_HPP
