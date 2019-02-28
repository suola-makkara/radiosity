#ifndef RADIOSITY_RENDER_ENGINE_HPP
#define RADIOSITY_RENDER_ENGINE_HPP

#include <vector>
#include <thread>
#include <mutex>

#include "plane.hpp"
#include "render_settings.hpp"
#include "shader.hpp"

class RadiosityRenderEngine
{
public:
	static void initialize();
	static void destruct();
	static void calculate();
	static void render(ShaderProgram &shaderProgram);
	static void stop();
	static bool isFinished();

	friend void LUDecompose(float **a, float **l, float **u,
			unsigned int n);
	friend void LUSolve(float *x, float *b, float **l, float **u,
			unsigned int n);
private:
	static std::vector<RadiosityPlane> objects;
	static std::vector<glm::vec3> radiosityVector;

	static std::mutex access;
	
	static bool initialized;
	static bool threadRunning;
	static bool shouldStop;
	static bool meshGenerated;
	static bool finished;

	static RenderSettings::FormFactorAlgorithm formFactorAlgorithm;
	static unsigned int samples;
	static RenderSettings::MatrixAlgorithm matrixAlgorithm;

	static void calculateInternal();
};

#endif // RADIOSITY_RENDER_ENGINE_HPP
