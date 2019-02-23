#ifndef MATRIX_OPERATIONS_HPP
#define MATRIX_OPERATIONS_HPP

extern float** genMatrix(unsigned int n);

extern void delMatrix(float **a, unsigned int n);

extern void copyMatrix(float **a, float **b, unsigned int n);

extern void identityMatrix(float **a, unsigned int n);

extern void LUDecompose(float **a, float **l, float **u, unsigned int n);

extern void LUSolve(float *x, float *b, float **l, float **u,
		unsigned int n);

#endif // MATRIX_OPERATIONS_HPP
