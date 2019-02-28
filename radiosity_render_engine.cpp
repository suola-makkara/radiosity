#include <mutex>
#include <iostream>

#include "radiosity_render_engine.hpp"
#include "scene.hpp"
#include "matrix_operations.hpp"

void RadiosityRenderEngine::initialize()
{
	std::lock_guard<std::mutex> lock(access);

	if (threadRunning) return;

	objects.clear();
	std::list<SubdividedPlane> &objs = Scene::getObjects();
	for (auto it = objs.begin(); it != objs.end(); it++)
		objects.push_back(*it);

	initialized = true;
	threadRunning = false;
	shouldStop = false;
	meshGenerated = false;
	finished = false;

	formFactorAlgorithm = RenderSettings::formFactorAlgorithm;
	samples = RenderSettings::formFactorSamples;
	matrixAlgorithm = RenderSettings::matrixAlgorithm;
}

void RadiosityRenderEngine::destruct()
{
	objects.clear();
}

void RadiosityRenderEngine::calculate()
{
	if (!initialized) return;

	std::thread(calculateInternal).detach();

	initialized = false;
	threadRunning = true;
}

void RadiosityRenderEngine::render(ShaderProgram &shaderProgram)
{
	std::lock_guard<std::mutex> lock(access);

	if (!finished) return;

	if (!meshGenerated)
	{
		unsigned int count = 0;
		for (int i = 0; i < objects.size(); i++)
		{
			objects[i].generateMesh(&radiosityVector[count]);
			count += objects[i].getElementCount();
		}
		radiosityVector.clear();
		meshGenerated = true;
	}

	for (auto it = objects.begin(); it != objects.end(); it++)
		it->draw(shaderProgram);
}

void RadiosityRenderEngine::stop()
{
	std::lock_guard<std::mutex> lock(access);
	
	threadRunning = false;
	shouldStop = true;
}

bool RadiosityRenderEngine::isFinished()
{
	std::lock_guard<std::mutex> lock(access);
	return finished;
}

std::vector<RadiosityPlane> RadiosityRenderEngine::objects =
	std::vector<RadiosityPlane>();
std::vector<glm::vec3> RadiosityRenderEngine::radiosityVector =
	std::vector<glm::vec3>();

std::mutex RadiosityRenderEngine::access;

bool RadiosityRenderEngine::initialized = false;
bool RadiosityRenderEngine::threadRunning = false;
bool RadiosityRenderEngine::shouldStop = false;
bool RadiosityRenderEngine::meshGenerated = false;
bool RadiosityRenderEngine::finished = false;

RenderSettings::FormFactorAlgorithm
	RadiosityRenderEngine::formFactorAlgorithm =
	RenderSettings::DIFF_AREA_TO_AREA_MONTE_CARLO;

unsigned int RadiosityRenderEngine::samples = 1;

RenderSettings::MatrixAlgorithm
	RadiosityRenderEngine::matrixAlgorithm =
	RenderSettings::LU_DECOMPOSITION;

void RadiosityRenderEngine::calculateInternal()
{
	unsigned int *elementIndexing = new unsigned int[objects.size()];
	unsigned int elementCount = 0u;
	for (int i = 0; i < objects.size(); i++)
	{
		elementIndexing[i] = elementCount;
		elementCount += objects[i].getElementCount();
	}

	float **formFactorMatrix = new float*[elementCount];
	for (int i = 0; i < elementCount; i++)
	{
		formFactorMatrix[i] = new float[elementCount];
		for (int j = 0; j < elementCount; j++)
			formFactorMatrix[i][j] = 0.0f;
	}

	switch (formFactorAlgorithm)
	{
	case RenderSettings::DIFF_AREA_TO_AREA_MONTE_CARLO:
		for (int i = 0; i < objects.size(); i++)
		{
			glm::vec3 normal = objects[i].getNormal();
			glm::vec3 tangent = objects[i].getTangent();
			glm::vec3 bitangent = objects[i].getBitangent();

			glm::mat3 objectNormalTransform(
					tangent.x, tangent.y, tangent.z,
					normal.x, normal.y, normal.z,
					bitangent.x, bitangent.y, bitangent.z);

			for (int e = 0; e < objects[i].getElementCount(); e++)
			{
				Ray ray;
				ray.origin = objects[i].getElementNodePosition(e);

				for (int s = 0; s < samples; s++)
				{
					float ru, rv;
					while (true)
					{
						ru = 2.0f * std::rand() / (float)RAND_MAX - 1.0f;
						rv = 2.0f * std::rand() / (float)RAND_MAX - 1.0f;
						if (std::pow(ru, 2) + std::pow(rv, 2) < 1.0f)
							break;
					}

					float rn = std::sqrt(1.0f -
								std::pow(ru, 2) - std::pow(rv, 2));

					ray.direction = objectNormalTransform *
						glm::normalize(glm::vec3(ru, rn, rv));

					float iu, iv, it;
					float mu, mv, mt = -1.0f;
					unsigned int mi;
					for (int j = 0; j < objects.size(); j++)
					{
						if (i == j) continue;

						if (objects[j].intersectsRay(ray, iu, iv, it))
						{
							if (mt == -1.0f || it < mt)
							{
								mt = it;
								mu = iu;
								mv = iv;
								mi = j;
							}
						}
					}

					if (mt == -1.0f)
						continue;

					formFactorMatrix[elementIndexing[mi] +
						objects[mi].getElementId(mu, mv)]
						[elementIndexing[i] + e] += 1.0f / samples;
				}

				std::lock_guard<std::mutex> lock(access);
				if (shouldStop)
				{
					delete[] elementIndexing;
					deleteMatrix(formFactorMatrix, elementCount);
					return;
				}
			}
		}
		break;
	}
	std::cout << "form factor done" << '\n';

	float *emissionVector[3];
	float *radiosityVector[3];
   	for (int i = 0; i < 3; i++)
	{
		emissionVector[i] = new float[elementCount];
		radiosityVector[i] = new float[elementCount];
	}

	for (int i = 0; i < objects.size(); i++)
		for (int j = 0; j < objects[i].getElementCount(); j++)
		{
			emissionVector[0][elementIndexing[i] + j]
				= objects[i].getEmission().x * 3.0f;
			emissionVector[1][elementIndexing[i] + j]
				= objects[i].getEmission().y * 3.0f;
			emissionVector[2][elementIndexing[i] + j]
				= objects[i].getEmission().z * 3.0f;
		}

	switch (matrixAlgorithm)
	{
	case RenderSettings::LU_DECOMPOSITION:
		{
		float **kMatrix = genMatrix(elementCount);
		float **lMatrix = genMatrix(elementCount);
		float **uMatrix = genMatrix(elementCount);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < objects.size(); j++)
				for (int k = 0; k < objects[j].getElementCount(); k++)
					for (int x = 0; x < elementCount; x++)
					{
						int y = elementIndexing[j] + k;
						kMatrix[x][y] = x == y ? 1.0f : 0.0f;
						kMatrix[x][y] -= formFactorMatrix[x][y] *
							objects[j].getColor()[i];
					}

			LUDecompose(kMatrix, lMatrix, uMatrix, elementCount);
			LUSolve(radiosityVector[i], emissionVector[i],
					lMatrix, uMatrix, elementCount);

			std::lock_guard<std::mutex> lock(access);
			if (shouldStop) break;
		}
		deleteMatrix(kMatrix, elementCount);
		deleteMatrix(lMatrix, elementCount);
		deleteMatrix(uMatrix, elementCount);
		deleteMatrix(formFactorMatrix, elementCount);

		delete[] elementIndexing;
		
		std::lock_guard<std::mutex> lock(access);
		if (shouldStop)
		{
			for (int i = 0; i < 3; i++)
			{
				delete[] emissionVector[i];
				delete[] radiosityVector[i];
			}
			return;
		}
		}
		break;
	}

	std::cout << "matrix done" << '\n';

	for (int i = 0; i < elementCount; i++)
		RadiosityRenderEngine::radiosityVector.push_back(glm::vec3(
					radiosityVector[0][i],
					radiosityVector[1][i],
					radiosityVector[2][i]));

	for (int i = 0; i < 3; i++)
	{
		delete[] emissionVector[i];
		delete[] radiosityVector[i];
	}

	std::lock_guard<std::mutex> lock(access);
	if (shouldStop) return;

	threadRunning = false;
	finished = true;
}
