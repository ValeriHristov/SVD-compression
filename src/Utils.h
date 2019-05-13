#pragma once
#include <vector>
#include "Matrix.h"

struct ImageData;



inline u32 alignTo4(u32 _num)
{
    return (_num + 3) & ~3;
}

void project(const MatrixValue* _u, const MatrixValue* _a, MatrixValue* _destination, u32 _size);

void multiplyVector(const MatrixValue* _vector, MatrixValue* _destination, u32 _size, MatrixValue _scalar);

void copyVector(const MatrixValue* _source, MatrixValue* _dest, u32 _size);

void normalizeVector(MatrixValue* _vector, u32 _size);

std::pair<MatrixValue, MatrixValue> getGivensCoeffs(MatrixValue _a, MatrixValue _b);

void applyGivensRotation(Matrix& _m, MatrixValue _c, MatrixValue _s, int _i, int _j);

std::vector<Matrix> imageToMatrices(const ImageData& _image);

std::vector<Matrix> QRAlgorithm(const Matrix& _matrix,u32 _iterations);

std::vector<Matrix> QRDecompose(const Matrix& _matrix);

std::vector<Matrix> QRGivensRotations(const Matrix & _matrix);

std::vector<Matrix> QRGramSchmidt(const Matrix& _matrix);

std::vector<Matrix> SVD(const Matrix& _matrix);