#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <algorithm>

#include "plane.hpp"
#include "settings.hpp"
#include "render_settings.hpp"

Plane::Plane(const Plane &plane)
	: Plane(plane.position, plane.rotation, plane.size,
			plane.color, plane.emission) { }

Plane::Plane(const glm::vec3 &position, const glm::vec3 &rotation,
		const glm::vec2 &size, const glm::vec3 &color,
		const glm::vec3 &emission)
	: position(position), size(size), color(color), emission(emission)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	setRotation(rotation);
}

Plane::~Plane()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

bool Plane::intersectsRay(const Ray &ray,
		float &u, float &v, float &t) const
{
	glm::vec3 pvec = glm::cross(ray.direction, vVec);

	float det = glm::dot(pvec, uVec);
	if (det < Settings::epsilon) return false;

	float invdet = 1.0f / det;

	glm::vec3 origin = position - 0.5f * uVec - 0.5f * vVec;
	glm::vec3 tvec = ray.origin - origin;

	u = invdet * glm::dot(pvec, tvec);
	if (u < 0.0f || u >= 1.0f) return false;

	glm::vec3 qvec = glm::cross(tvec, uVec);

	v = invdet * glm::dot(qvec, ray.direction);
	if (v < 0.0f || v >= 1.0f) return false;

	t = invdet * glm::dot(qvec, vVec);
	return t >= 0.0f;
	return false;
}

void Plane::draw(ShaderProgram &shaderProgram) const
{
	shaderProgram.use();
	shaderProgram.set3fv("color", color);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void setVec3(float *dest, float *source);

void Plane::updateMesh()
{
	glm::vec3 v0 = position - 0.5f * uVec - 0.5f * vVec;
	glm::vec3 v1 = position + 0.5f * uVec - 0.5f * vVec;
	glm::vec3 v2 = position + 0.5f * uVec + 0.5f * vVec;
	glm::vec3 v3 = position - 0.5f * uVec + 0.5f * vVec;
	
	glm::vec3 verticesVec[6] { v0, v1, v3, v1, v2, v3 };

	float vertices[18];

	for (int i = 0; i < 6; i++)
		setVec3(&vertices[3 * i], &verticesVec[i].x);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
			vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

glm::vec3 Plane::getPosition() const { return position; }
glm::vec3 Plane::getRotation() const { return rotation; }
glm::vec2 Plane::getSize() const { return size; }

void Plane::setPosition(const glm::vec3 &position)
{
	this->position = position;
	updateMesh();
}

void Plane::setRotation(const glm::vec3 &rotation)
{
	this->rotation = rotation;

	uVec = glm::vec3(size.x, 0.0f, 0.0f);
	vVec = glm::vec3(0.0f, size.y, 0.0f);

	glm::mat4 r(1);
	r = glm::rotate(r, glm::radians(rotation.x), glm::vec3(0, 1, 0));
	r = glm::rotate(r, glm::radians(rotation.y), glm::vec3(1, 0, 0));
	r = glm::rotate(r, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	uVec = glm::mat3(r) * uVec;
	vVec = glm::mat3(r) * vVec;
	
	updateMesh();
}

void Plane::setSize(const glm::vec2 &size)
{
	glm::vec2 newSize;
	newSize.x = std::max(size.x, 0.0001f);
	newSize.y = std::max(size.y, 0.0001f);

	uVec *= newSize.x / this->size.x;
	vVec *= newSize.y / this->size.y;
	
	this->size = newSize;
	updateMesh();
}

glm::vec3 Plane::getColor() const { return color; }
glm::vec3 Plane::getEmission() const { return emission; }

void Plane::setColor(const glm::vec3 &color) { this->color = color; }
void Plane::setEmission(const glm::vec3 &emission) {
	this->emission = emission; }


SubdividedPlane::SubdividedPlane(const SubdividedPlane &plane)
	: SubdividedPlane(plane.getPosition(), plane.getRotation(),
			plane.getSize(), plane.getColor(), plane.getEmission()) { }

SubdividedPlane::SubdividedPlane(
			const glm::vec3 &position, const glm::vec3 &rotation,
			const glm::vec2 &size, const glm::vec3 &color,
			const glm::vec3 &emission)
	: Plane(position, rotation, size, color, emission)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	updateMesh();
}

SubdividedPlane::~SubdividedPlane()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void SubdividedPlane::draw(ShaderProgram &shaderProgram) const
{
	shaderProgram.use();
	shaderProgram.set3fv("color", glm::vec3(1.0f, 1.0f, 1.0f));

	glBindVertexArray(VAO);

	unsigned int lines = meshWidth + meshHeight + 2;
	glDrawArrays(GL_LINES, 0, lines * 2);
}

void SubdividedPlane::updateMesh()
{
	Plane::updateMesh();

	meshWidth = std::max(std::min(RenderSettings::subdivisionLevel,
				(unsigned int)std::floor(size.x /
					RenderSettings::maxSubdivisionSize)), 1u);
	meshHeight = std::max(std::min(RenderSettings::subdivisionLevel,
				(unsigned int)std::floor(size.y /
					RenderSettings::maxSubdivisionSize)), 1u);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float *vertices = new float[(meshWidth + meshHeight + 2) * 6];

	glm::vec3 origin = position - 0.5f * uVec - 0.5f * vVec;

	float dx = 1.0f / meshWidth;
	float dy = 1.0f / meshHeight;

	for (int x = 0; x <= meshWidth; x++)
	{
		glm::vec3 v0 = origin + x * dx * uVec;
		glm::vec3 v1 = v0 + vVec;
		setVec3(&vertices[x * 6], &v0.x);
		setVec3(&vertices[x * 6 + 3], &v1.x);
	}

	for (int y = 0; y <= meshHeight; y++)
	{
		glm::vec3 v0 = origin + y * dy * vVec;
		glm::vec3 v1 = v0 + uVec;
		setVec3(&vertices[(meshWidth + 1 + y) * 6], &v0.x);
		setVec3(&vertices[(meshWidth + 1 + y) * 6 + 3], &v1.x);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
			GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
			(void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	
	delete[] vertices;
}

void SubdividedPlane::setPosition(const glm::vec3 &position)
{
	Plane::setPosition(position);
	updateMesh();
}

void SubdividedPlane::setRotation(const glm::vec3 &rotation)
{
	Plane::setRotation(rotation);
	updateMesh();
}

void SubdividedPlane::setSize(const glm::vec2 &size)
{
	Plane::setSize(size);
	updateMesh();
}


RadiosityPlane::RadiosityPlane(const RadiosityPlane &plane)
	: RadiosityPlane((const SubdividedPlane)plane) { }

RadiosityPlane::RadiosityPlane(const SubdividedPlane &plane)
	: SubdividedPlane(plane)
{
	normal = glm::normalize(glm::cross(uVec, vVec));

	meshGenerated = false;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

RadiosityPlane::~RadiosityPlane()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void RadiosityPlane::draw(ShaderProgram &shaderProgram) const
{
	if (!meshGenerated) return;

	shaderProgram.use();
	glBindVertexArray(VAO);
	
	glDrawArrays(GL_TRIANGLES, 0, getElementCount() * 6);
}

void RadiosityPlane::generateMesh(glm::vec3 *radiosityVector)
{
	glm::vec3 origin = position - 0.5f * uVec - 0.5f * vVec;

	float *vertices = new float[getElementCount() * 36];

	glm::vec3 dx = uVec / (float)meshWidth;
	glm::vec3 dy = vVec / (float)meshHeight;
	for (int x = 0; x < meshWidth; x++)
		for (int y = 0; y < meshHeight; y++)
		{
			glm::vec3 v0 = origin + (float)x * dx + (float)y * dy;
			glm::vec3 v1 = v0 + dx;
			glm::vec3 v2 = v0 + dy;
			glm::vec3 v3 = v1 + dy;

			setVec3(&vertices[(x + y * meshWidth) * 36], &v0.x);
			setVec3(&vertices[(x + y * meshWidth) * 36 + 6], &v1.x);
			setVec3(&vertices[(x + y * meshWidth) * 36 + 12], &v2.x);
			setVec3(&vertices[(x + y * meshWidth) * 36 + 18], &v1.x);
			setVec3(&vertices[(x + y * meshWidth) * 36 + 24], &v3.x);
			setVec3(&vertices[(x + y * meshWidth) * 36 + 30], &v2.x);

			glm::vec3 temp(1.0f);
			for (int i = 0; i < 6; i++)
				setVec3(&vertices[(x + y * meshWidth) * 36 + 3 + 6 * i],
						&radiosityVector[(x + y * meshWidth)].x);
		}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
			GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
			(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
			(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	meshGenerated = true;

	delete[] vertices;
}

unsigned int RadiosityPlane::getElementCount() const {
	return meshWidth * meshHeight; }

unsigned int RadiosityPlane::getElementId(float u, float v) const
{
	return std::floor(u * meshWidth) +
		std::floor(v * meshHeight) * meshWidth;
}

glm::vec3 RadiosityPlane::getElementNodePosition(unsigned int id) const
{
	return position + uVec * ((0.5f + id % meshWidth) / meshWidth - 0.5f)
		+ vVec * ((0.5f + id / meshWidth) / meshHeight - 0.5f);
}

glm::vec3 RadiosityPlane::getNormal() const { return normal; }

glm::vec3 RadiosityPlane::getTangent() const {
	return glm::normalize(uVec); }

glm::vec3 RadiosityPlane::getBitangent() const {
	return glm::normalize(vVec); }


void setVec3(float *dest, float *source)
{
	for (int i = 0; i < 3; i++)
		dest[i] = source[i];
}
