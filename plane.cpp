#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

#include "plane.hpp"

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
	// TODO Implement
	return false;
}

void Plane::draw(ShaderProgram &shaderProgram) const
{
	shaderProgram.use();
	shaderProgram.set3fv("color", color);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Plane::updateMesh()
{
	glm::vec3 v0 = position - 0.5f * uVec - 0.5f * vVec;
	glm::vec3 v1 = position + 0.5f * uVec - 0.5f * vVec;
	glm::vec3 v2 = position + 0.5f * uVec + 0.5f * vVec;
	glm::vec3 v3 = position - 0.5f * uVec + 0.5f * vVec;
	
	glm::vec3 verticesVec[6] { v0, v1, v3, v1, v2, v3 };

	float vertices[18];

	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 3; j++)
			vertices[3 * i + j] = verticesVec[i][j];

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



















/*
// TODO documentation

Plane::Plane() : origin(0.5f, 0.5f, 0.0f),
	edge1(1.0f, 0.0f, 0.0f), edge2(0.0f, 1.0f, 0.0f),
	material{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f } { }

Plane::Plane(const glm::vec3 &origin, const glm::vec3 &rotation,
		const glm::vec2 &size, const Material &material)
	: material(material)
{
	glm::mat4 r(1);
	r = glm::rotate(r, rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
	r = glm::rotate(r, rotation.y, glm::vec3(0.0f, 0.0f, 1.0f));
	r = glm::rotate(r, rotation.z, glm::vec3(1.0f, 0.0f, 0.0f));

	edge1 = glm::mat3(r) * (size.x * glm::vec3(1.0f, 0.0f, 0.0f));
	edge2 = glm::mat3(r) * (size.y * glm::vec3(0.0f, 1.0f, 0.0f));

	this->origin = origin - 0.5f * (edge1 + edge2);

	std::lock_guard<std::mutex> lock(RenderSettings::access);

	width = std::max(std::min(RenderSettings::subdivisionLevel,
				(int)std::floor(glm::length(edge1) /
					RenderSettings::minSubdivisionSize)), 1);
	
	height = std::max(std::min(RenderSettings::subdivisionLevel,
				(int)std::floor(glm::length(edge2) /
					RenderSettings::minSubdivisionSize)), 1);

	generateDrawData();
}

Plane::~Plane()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Plane::generateDrawData()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, );

	glVertexAttribPointer(0, );
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Plane::render(GLFWwindow &window, ShaderProgram &shaderProgram)
{
	shaderProgram.use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, getElements() * 6, GL_UNSIGNED_INT, 0);
}

bool Plane::intersectsRay(const Ray &ray,
		float &u, float &v, float &t) const
{
	// TODO create epsilon based on editor grid size and replace hardcoded values

	glm::vec3 pvec = glm::cross(ray.direction, edge2);

	float det = glm::dot(pvec, edge1);
	if (det < 0.001f) return false;

	float invdet = 1.0f / det;

	glm::Vec3 tvec = ray.origin - origin;

	u = incdet * glm::dot(pvec, tvec);
	if (u < 0.0f || u >= 1.0f) return false;

	glm::vec3 qvec = glm::cross(tvec, edge1);

	v = invdet * glm::dot(qvec, ray.direction);
	if (v < 0.0f || v >= 1.0f) return false;

	t = invdet * glm::dot(qvec, edge2);
	return t >= 0.0f;
}

unsigned int Plane::getWidth() const { return width; }
unsigned int Plane::getHeight() const { return height; }

unsigned int Plane::getElementCount() const
{
	return width * height;
}

unsigned int Plane::getElementID(float u, float v) const
{
	return std::floor(u * width) + std::floor(v * height) * width;
}

glm::vec3 Plane::getPosition(float u, float v) const
{
	return edge1 * u + edge2 * v;
}

glm::vec3 Plane::getElementNodePosition(
		unsigned int elementID) const
{
	float d1 = edge1 / (width * 2.0f);
	float d2 = edge2 / (height * 2.0f);

	return d1 * ((elementID % width) * 2.0f + 1.0f) +
		d2 * (elementID / width * 2.0f + 1.0f);
}

std::vector<glm::vec2> Plane::getTriangleStripUV() const
{
	float du = 1.0f / width;
	float dv = 1.0f / height;

	std::vector<glm::vec2> triangleStrip;
	triangleStrip.reserve(height * (width * 2 + 2));

	for (int y = 0; y < hiehgt; y++)
	{
		if (y % 2) for (int x = width - 1; x >= 0; x--)
		{
			if (x == width - 1)
			{
				triangleStrip.push_back({ (x + 1) * du, y * dv });
				triangleStrip.push_back({ (x + 1) * du, (y + 1) * dv });
			}

			triangleStrip.push_back({ x * du, y * dv });
			triangleStrip.push_back({ x * du, (y + 1) * dv });
		}
		else for (int x = 0; x < width; x++)
		{
			if (x == 0)
			{
				triangleStrip.push_back({ x * du, (y + 1) * dv });
				triangleStrip.push_back({ x * du, y * dv });
			}
			
			triangleStrip.push_back({ (x + 1) * du, y * dv });
			triangleStrip.push_back({ (x + 1) * du, (y + 1) * dv });
		}
	}

	return triangleStrip;
}

const Material& Plane::getMaterial() const { return material; }

RadiosityPlane::RadiosityPlane(
		const Plane &plane, glm::vec3 *radiosities)
	: Plane(plane)
{
	this->radiosities = new glm::vec3[getElementCount()];
	for (int i = 0; i < getElementCount(); i++)
		this->radiosities[i] = radiosities[i];

	generateDrawData();
}

RadiosityPlane::~RadiosityPlane()
{
	if (radiosities != nullptr) delete[] radiosities;
}

RadiosityPlane::generateDrawData()
{
}

RadiosityPlane::render(GLFWwindow &window, ShaderProgram &shaderProgram)
{
}

glm::vec3 RadiosityPlane::getRadiosity(float u, float v) const
{
	unsigned int interpolationFunction;

	{
		std::lock_guard<std::mutex> lock(RenderSettings::access);
		interpolationFunction =
			RenderSettings::radiosityValueInterpolationFunction;
	}

	// TODO Special case width or hiehgt = 1
	
	float vu = std::round(u * width);
	float vv = std::round(v * height);

	float u0 = vu - 0.5f;
	float u1 = vu + 0.5f;
	if (u0 < 0.0f) u0 = u1, u1 = vu + 1.5f;
	else if (u1 > width) u1 = u0, u0 = vu - 1.5f;

	float v0 = vv - 0.5f;
	float v1 = vv + 0.5f;
	if (v0 < 0.0f) v0 = v1, v1 = vv + 1.5f;
	else if (v1 > hieght) v1 = v0, v0 = vv - 1.5f;

	x = u * width - u0;
	y = v * height - v0;

	u0 /= width, u1 /= width;
	v0 /= height, v1 /= height;

	glm::vec3 r0 = radiosities(u0, v0);
	glm::vec3 r1 = radiosities(u1, v0);
	glm::vec3 r2 = radiosities(u0, v1);
	glm::vec3 r3 = radiosities(u1, v1);

	switch (interpolationFunction)
	{
	case RenderSettings::LINEAR:
		{
			glm::vec3 r4 = (r0 - r1) * x + r0;
			glm::vec3 r5 = (r2 - r3) * x + r2;

			return (r4 - r5) * y + r4;
		}
		break;
	case RenderSettings::CUBIC:
		// TODO implement cubic interpolation
		return glm::vec3(0.0f);
		break;
	}
}*/
