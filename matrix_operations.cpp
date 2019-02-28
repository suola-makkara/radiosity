#include "matrix_operations.hpp"
#include "radiosity_render_engine.hpp"

float** genMatrix(unsigned int n)
{
	float **a = new float*[n];
	for (int i = 0; i < n; i++)
		a[i] = new float[n];
	return a;
}

void deleteMatrix(float** a, unsigned int n)
{
	for (int i = 0; i < n; i++)
		delete[] a[i];
	delete[] a;
}

void copyMatrix(float **a, float **b, unsigned int n)
{
	for (int x = 0; x < n; x++)
		for (int y = 0; y < n; y++)
			b[x][y] = a[x][y];
}

void identityMatrix(float **a, unsigned int n)
{
	for (int x = 0; x < n; x++)
		for (int y = 0; y < n; y++)
			a[x][y] = x == y ? 1 : 0;
}

void LUDecompose(float **a, float **l, float **u, unsigned int n)
{
	copyMatrix(a, u, n);
	identityMatrix(l, n);
	for (int x = 0; x < n - 1; x++)
		for (int y = x + 1; y < n; y++)
		{
			float elimination = -u[x][y]/a[x][x];

			u[x][y] = 0.0f;
			for (int i = x + 1; i < n; i++)
				u[i][y] += elimination * u[i][x];

			l[x][y] = -elimination;
			
			std::lock_guard<std::mutex>
				lock(RadiosityRenderEngine::access);
			if (RadiosityRenderEngine::shouldStop) return;
		}
}

void LUSolve(float *x, float *y, float **l, float **u, unsigned int n)
{
	float *z = new float[n];

	for (int i = 0; i < n; i++)
	{
		z[i] = y[i];
		for (int j = 0; j < i; j++)
			z[i] -= l[j][i] * z[j];

		z[i] /= l[i][i];
		
		std::lock_guard<std::mutex>
			lock(RadiosityRenderEngine::access);
		if (RadiosityRenderEngine::shouldStop) return;
	}

	for (int i = n - 1; i >= 0; i--)
	{
		x[i] = z[i];
		for (int j = i + 1; j < n; j++)
			x[i] -= u[j][i] * x[j];

		x[i] /= u[i][i];
		
		std::lock_guard<std::mutex>
			lock(RadiosityRenderEngine::access);
		if (RadiosityRenderEngine::shouldStop) return;
	}

	delete[] z;
}
