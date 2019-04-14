#pragma once
#include <vector>
#include "Matrix.h"

struct ImageData;



inline u32 alignTo4(u32 _num)
{
    return (_num + 3) & ~3;
}

void project(const float* _u, const float* _a, float* _destination, u32 _size);

void multiplyVector(const float* _vector, float* _destination, u32 _size, float _scalar);

void copyVector(const float* _source, float* _dest, u32 _size);

void normalizeVector(float* _vector, u32 _size);

std::vector<Matrix> imageToMatrices(const ImageData& _image);

std::vector<Matrix> QRAlgorithm(const Matrix& _matrix,u32 _iterations);

std::vector<Matrix> QRDecompose(const Matrix& _matrix);

std::vector<Matrix> QRGramSchmidt(const Matrix& _matrix);

std::vector<Matrix> SVD(const Matrix& _matrix);