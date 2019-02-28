#include "render_settings.hpp"

unsigned int RenderSettings::subdivisionLevel = 1u;
float RenderSettings::maxSubdivisionSize = 0.25f;

RenderSettings::FormFactorAlgorithm RenderSettings::formFactorAlgorithm =
	RenderSettings::DIFF_AREA_TO_AREA_MONTE_CARLO;

unsigned int RenderSettings::formFactorSamples = 1u;

RenderSettings::MatrixAlgorithm RenderSettings::matrixAlgorithm =
	RenderSettings::LU_DECOMPOSITION;
