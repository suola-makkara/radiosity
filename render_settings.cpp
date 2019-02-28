#include "render_settings.hpp"

unsigned int RenderSettings::subdivisionLevel = 8u;
float RenderSettings::maxSubdivisionSize = 0.5f;

RenderSettings::FormFactorAlgorithm RenderSettings::formFactorAlgorithm =
	RenderSettings::DIFF_AREA_TO_AREA_MONTE_CARLO;

unsigned int RenderSettings::formFactorSamples = 5000u;

RenderSettings::MatrixAlgorithm RenderSettings::matrixAlgorithm =
	RenderSettings::LU_DECOMPOSITION;

RenderSettings::Shading RenderSettings::shading = RenderSettings::GOURAUD;
