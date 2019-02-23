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

	static std::list<Plane>& getObjects();

	static void addObject(const Plane &plane);

	static void removeObject(std::list<Plane>::iterator &it);

	static void destruct();
private:
	static std::string file;
	
	static bool saved;

	static std::list<Plane> objects;
};

#endif // SCENE_HPP
