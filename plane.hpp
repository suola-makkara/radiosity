#ifndef PLANE_HPP
#define PLANE_HPP

#include <glm/glm.hpp>

#include <vector>

#include "ray.hpp"
#include "shader.hpp"

class Plane
{
public:
	Plane(const Plane &plane);

	Plane(
			const glm::vec3 &position = glm::vec3(0.0f),
			const glm::vec3 &rotation = glm::vec3(0.0f),
			const glm::vec2 &size = glm::vec2(1.0f),
			const glm::vec3 &color = glm::vec3(1.0f),
			const glm::vec3 &emission = glm::vec3(0.0f));


	~Plane();

	bool intersectsRay(const Ray &ray, float &u, float &v, float &t) const;

	void draw(ShaderProgram &shaderProgram) const;

	void updateMesh();

	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::vec2 getSize() const;

	void setPosition(const glm::vec3 &position);
	void setRotation(const glm::vec3 &rotation);
	void setSize(const glm::vec2 &size);

	glm::vec3 getColor() const;
	glm::vec3 getEmission() const;

	void setColor(const glm::vec3 &color);
	void setEmission(const glm::vec3 &emission);
protected:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec2 size;

	glm::vec3 color;
	glm::vec3 emission;

	// Used for raytracing and drawing
	glm::vec3 uVec;
	glm::vec3 vVec;
private:
	unsigned int VAO;
	unsigned int VBO;
};

class SubdividedPlane : public Plane
{
public:
	SubdividedPlane(const SubdividedPlane &plane);
	SubdividedPlane(
			const glm::vec3 &position = glm::vec3(0.0f),
			const glm::vec3 &rotation = glm::vec3(0.0f),
			const glm::vec2 &size = glm::vec2(1.0f),
			const glm::vec3 &color = glm::vec3(1.0f),
			const glm::vec3 &emission = glm::vec3(0.0f));

	~SubdividedPlane();

	void draw(ShaderProgram &shaderProgram) const;
	
	void updateMesh();
	
	void setPosition(const glm::vec3 &position);
	void setRotation(const glm::vec3 &rotation);
	void setSize(const glm::vec2 &size);
protected:
	unsigned int meshWidth;
	unsigned int meshHeight;
private:
	unsigned int VAO;
	unsigned int VBO;
};

class RadiosityPlane : public SubdividedPlane
{
public:
	RadiosityPlane(const RadiosityPlane &plane);
	RadiosityPlane(const SubdividedPlane &plane);

	~RadiosityPlane();
	
	void draw(ShaderProgram &shaderProgram) const;

	void generateMesh(glm::vec3 *radiosityVector);

	unsigned int getElementCount() const;

	unsigned int getElementId(float u, float v) const;

	glm::vec3 getElementNodePosition(unsigned int id) const;

	glm::vec3 getNormal() const;
	glm::vec3 getTangent() const;
	glm::vec3 getBitangent() const;
private:
	glm::vec3 normal;

	bool meshGenerated;
	unsigned int VAO;
	unsigned int VBO;
};

#endif // PLANE_HPP
