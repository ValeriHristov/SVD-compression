#pragma once
#include <vector>
#include "Matrix.h"

struct ImageData;

enum PixelConvertion
{
    Mul255, // [0,1] => [0,255]
    Div255, // [0,255] => [0,1]
    None
};

template<class T>
void clamp(T& value, T from, T to)
{
    if (value < from) value = from;
    else if (value > to) value = to;
}

template<class T>
u8 pixelConvert(T _pixel, PixelConvertion _method)
{
    clamp<T>(_pixel, 0.0, 255.0);
    if (_method == None)
        return (u8)_pixel;
    if (_method == Mul255)
        return (u8)_pixel*255.0f;
    if (_method == Div255)
        return (u8)_pixel / 255.0f;
    _ASSERT(0);
}

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

ImageData matricesToImage(const std::vector<Matrix>& _matrices, PixelConvertion _convertion);

std::vector<Matrix> QRAlgorithm(const Matrix& _matrix,u32 _iterations);

std::vector<Matrix> QRDecompose(const Matrix& _matrix);

std::vector<Matrix> QRGivensRotations(const Matrix & _matrix);

std::vector<Matrix> QRGramSchmidt(const Matrix& _matrix);

std::vector<Matrix> SVD(const Matrix& _matrix);

Matrix compress(const std::vector<Matrix>& _svd, u32 _valuesToRemove);

void draw(const ImageData& _image);