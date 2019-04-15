#include "Utils.h"
#include "IImageReader.h"
#include "ScopedTimer.h"

#pragma warning(disable::4996)

template<class T>
T min(T a, T b)
{
    return a < b ? a : b;
}

void project(const float * _u, const float * _a, float * _destination, u32 _size)
{
    float multiplier = dotProduct(_u, _a, _size) / dotProduct(_u, _u, _size);
    multiplyVector(_u, _destination, _size, multiplier);
}

void multiplyVector(const float * _vector, float * _destination, u32 _size, float _scalar)
{
    for (u32 i = 0; i < _size; i++)
    {
        _destination[i] = _scalar * _vector[i];
    }
}

void copyVector(const float * _source, float * _dest, u32 _size)
{
    for (u32 i = 0; i < _size; i++)
    {
        _dest[i] = _source[i];
    }
}

void subtractVectors(const float* _left, const float* _right, float* _dest, u32 _size)
{
    for (int i = 0; i < _size; i++)
    {
        _dest[i] = _left[i] - _right[i];
    }
}

void normalizeVector(float* _vector, u32 _size)
{
    float len = sqrt(dotProduct(_vector, _vector, _size));
    multiplyVector(_vector, _vector, _size, 1 / len);
}

std::vector<Matrix> imageToMatrices(const ImageData & _image)
{
    //TODO support different pixel formats
    //now, assume 24bit RGB -> 3 matrices

    const u8 matrixCount = 3u;

    std::vector<Matrix> matrices(matrixCount);
    std::fill(matrices.begin(), matrices.end(), Matrix(_image.m_height, _image.m_width));

    u8* data = static_cast<u8*>(_image.m_pixels.m_data);


    u32 index = 0;

    for (u32 i = 0u; i < _image.m_height; i++)
    {
        for (u32 j = 0u; j < _image.m_width; j++)
        {
            for (u32 m = 0u; m < matrixCount; m++)
            {
                matrices[m][i][j] = static_cast<MatrixValue>(data[index++]);
            }
        }
        index = alignTo4(index);
    }



    return std::move(matrices);
}

std::vector<Matrix> QRAlgorithm(const Matrix & _matrix, u32 _iterations)
{
    std::vector<Matrix> qs;
    qs.reserve(_iterations);

    Matrix A = _matrix;
    for (int i = 0; i < _iterations; i++)
    {
        auto qr = QRGramSchmidt(A);
        A = qr[1] * qr[0];
        qs.emplace_back(std::move(qr[0]));
    }

    Matrix Q = std::move(qs[0]);
    u32 count = qs.size();

    for (int i = 1; i < count; i++)
    {
        Q = Q * qs[i];
    }

    std::vector<Matrix> res;
    res.push_back(std::move(A)); // eigenvalues
    res.push_back(std::move(Q)); // eigenvectors by rows

    return std::move(res);
}

std::vector<Matrix> QRGramSchmidt(const Matrix & _matrix)
{
    Matrix m = _matrix.transpose();

    u32 vectorsCount = m.getRows();
    u32 vectorLen = m.getCols();
    Matrix q(vectorsCount, vectorLen);

    float* temp = new float[vectorLen];

    copyVector(m[0], q[0], vectorLen);

    for (u32 i = 1; i < vectorsCount; i++)
    {
        copyVector(m[i], q[i], vectorLen);
        for (u32 j = 0; j < i; j++)
        {
            project(q[j], m[i], temp, vectorLen);
            subtractVectors(q[i], temp, q[i], vectorLen);
        }
    }

    delete[] temp;

    for (u32 i = 0; i < vectorsCount; i++)
    {
        normalizeVector(q[i], vectorLen);
    }

    std::vector<Matrix> qr;
    qr.push_back(q.transpose());
    qr.push_back(q * _matrix);

    return std::move(qr);
}

std::vector<Matrix> SVD(const Matrix & _matrix)
{
    if (_matrix.getRows() < _matrix.getCols())
    {
        auto svd = SVD(_matrix.transpose());
        std::vector<Matrix> result;
        result.push_back(svd[2].transpose());
        result.push_back(std::move(svd[1]));
        result.push_back(svd[0].transpose());
        return result;
    }

    const Matrix& A = _matrix;

    std::vector<Matrix> qr;
    qr = QRAlgorithm(A.transpose() * A, 15);
    Matrix s = std::move(qr[0]);
    Matrix u = std::move(qr[1]);

    for (u16 i = 0; i < s.getRows(); i++)
    {
        for (int j = 0; j < s.getCols(); j++)
        {
            if (i == j)
                s[i][i] = sqrt(s[i][i]);
            else
                s[i][j] = 0;
        }
    }

    Matrix S = s;
    //  u16 eigenValueCount = min(s.getCols(), s.getRows());
    //
    //  Matrix S(A.getRows(), A.getCols());
    //
    //  for (u16 i = 0; i < S.getRows(); i++)
    //  {
    //      for (int j = 0; j < S.getCols(); j++)
    //      {
    //          if (i == j && i < eigenValueCount)
    //              S[i][i] = sqrt(s[i][i]);
    //          else
    //              S[i][j] = 0;
    //      }
    //  }

      // From : http://www.netlib.org/utk/people/JackDongarra/etemplates/node40.html
      // At * ui = si * vi 
    Matrix v = (A*u).transpose();
    {
        for (u32 i = 0; i < v.getRows(); i++)
        {
            multiplyVector(v[i], v[i], v.getCols(), 1 / S[i][i]);
        }
    }
    std::vector<Matrix> svd;

    svd.push_back(v.transpose());
    svd.push_back(S);
    svd.push_back(u);

    return std::move(svd);
}
