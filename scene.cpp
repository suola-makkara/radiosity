#include <fstream>
#include <sstream>
#include <iostream>

#include "scene.hpp"

void Scene::loadNew()
{
	file = "";
	saved = false;
	objects = std::list<Plane>();
}

void Scene::load(const std::string &file)
{
	Scene::file = file;

	std::ifstream stream(file);

	// Load scene
	std::cout << "Loading from: " << file << '\n';

	saved = true;
}

void Scene::save()
{
	std::ofstream stream(file);

	// Save scene
	std::cout << "Saving to: " << file << '\n';

	saved = true;
}

void Scene::saveAs(const std::string &file)
{
	Scene::file = file;
	save();
}

bool Scene::hasFileAddress() { return !file.empty(); }

bool Scene::isSaved() { return saved; }

void Scene::update() { saved = false; }

std::list<Plane>& Scene::getObjects() { return objects; }

void Scene::addObject(const Plane &plane)
{
	objects.push_back(plane);
	saved = false;
}

void Scene::removeObject(std::list<Plane>::iterator &it)
{
	objects.erase(it);
	saved = false;
}

void Scene::destruct() { objects.clear(); }

std::string Scene::file = "";

bool Scene::saved = false;

std::list<Plane> Scene::objects = std::list<Plane>();
