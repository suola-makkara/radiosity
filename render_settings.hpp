#ifndef RENDER_SETTINGS_HPP
#define RENDER_SETTINGS_HPP

struct RenderSettings
{
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
		LU_DECOMPOSITION
	};

	static MatrixAlgorithm matrixAlgorithm;

	enum Shading
	{
		CELL,
		GOURAUD
	};

	static Shading shading;
};

#endif // RENDER_SETTINGS_HPP
