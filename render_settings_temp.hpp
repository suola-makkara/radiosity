#ifndef RENDER_SETTINGS_HPP
#define RENDER_SETTINGS_HPP

#include <mutex>

struct RenderSettings
{
	static std::mutex access;

	static bool wireframe;
	static bool cullFace;

	static unsigned int subdivisionLevel;
	static float maxSubdivisionSize;

	enum FormFactorAlgorithm
	{
		DIFF_AREA_TO_AREA_MONTE_CARLO
	};

	static FormFactorAlgorithm formFactorAlgorithm;

	static unsigned int formFactorSamples;

	enum MatrixAlgorithm
	{
		LUDecomposition
	};

	static MatrixAlgorithm matrixAlgorithm;

	enum RadiosityValueInterpolationFunction
	{
		LINEAR,
		CUBIC
	}

	static RadiosityValueInterpolationFunction
		radiosityValueInterpolationFunction;
};

#endif // RENDER_SETTINGS_HPP
