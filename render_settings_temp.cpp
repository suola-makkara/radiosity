#include "render_settings_test.hpp"

RenderSettings::wireframe = false;
RenderSettings::cullFace = true;

RenderSettings::subdivisionLevel = 8u;
RenderSettings::maxSubdivisionSize = 0.25f;

RenderSettings::formFactorAlgorithm =
	RenderSettings::DIFF_AREA_TO_AREA_MONTE_CARLO;

RenderSettings::formFactorSamples = 10000u;

RenderSettings::matrixAlgorithm =
	RenderSettings::LUDecomposition;
