#include <fstream>
#include <sstream>
#include <iostream>

#include "scene.hpp"
#include "editor.hpp"

void Scene::loadNew()
{
	file = "";
	saved = false;
	Editor::reset();
	objects = std::list<Plane>();
}

glm::vec2 readVec2(std::stringstream &ss);
glm::vec3 readVec3(std::stringstream &ss);

void Scene::load(const std::string &file)
{
	std::ifstream stream(file);

	if (!stream.is_open()) return;

	Scene::file = file;
	Editor::reset();
	objects = std::list<Plane>();

	std::string s;
	std::stringstream ss;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec2 size;

	glm::vec3 color;
	glm::vec3 emission;

	while (std::getline(stream, s))
	{
		if (s.empty()) break;

		ss << s;

		position = readVec3(ss);
		rotation = readVec3(ss);
		size = readVec2(ss);

		color = readVec3(ss);
		emission = readVec3(ss);

		objects.push_back(Plane(position, rotation, size,
					color, emission));
	}

	stream.close();

	saved = true;
}

void writeVec2(const glm::vec2 &vec, std::ofstream &stream);
void writeVec3(const glm::vec3 &vec, std::ofstream &stream);

void Scene::save()
{
	std::ofstream stream(file);

	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		writeVec3(it->getPosition(), stream);
		writeVec3(it->getRotation(), stream);
		writeVec2(it->getSize(), stream);
		
		writeVec3(it->getColor(), stream);
		writeVec3(it->getEmission(), stream);

		stream << '\n';
	}

	stream.close();

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

void Scene::removeObject(Plane *plane)
{
	for (auto it = objects.begin(); it != objects.end(); it++)
		if (&(*it) == plane)
		{
			objects.erase(it);
			break;
		}

	saved = false;
}

void Scene::destruct() { objects.clear(); }


std::string Scene::file = "";

bool Scene::saved = false;

std::list<Plane> Scene::objects = std::list<Plane>();


glm::vec2 readVec2(std::stringstream &ss)
{
	glm::vec2 vec;
	for (int i = 0; i < 2; i++)
		ss >> vec[i];
	return vec;
}

glm::vec3 readVec3(std::stringstream &ss)
{
	glm::vec3 vec;
	for (int i = 0; i < 3; i++)
		ss >> vec[i];
	return vec;
}

void writeVec2(const glm::vec2 &vec, std::ofstream &stream)
{
	for (int i = 0; i < 2; i++)
		stream << vec[i] << " ";
}

void writeVec3(const glm::vec3 &vec, std::ofstream &stream)
{
	for (int i = 0; i < 3; i++)
		stream << vec[i] << " ";
}
