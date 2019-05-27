#include <windows.h>

#include "Utils.h"
#include "IImageReader.h"
#include "ScopedTimer.h"

std::pair<MatrixValue, MatrixValue> getGivensCoeffs(MatrixValue a, MatrixValue b)
{
    MatrixValue c, s;
    MatrixValue r = sqrt(a*a + b *b);
    c = a / r;
    s = -b / r;
    return std::make_pair(c, s);
}

void applyGivensRotation(Matrix& _m, MatrixValue _c, MatrixValue _s, int _targetRow, int _startCol)
{
    for (int col = _startCol ; col < _m.getCols(); col++)
    {
        MatrixValue val1 = _m[_targetRow - 1][col];
        MatrixValue val2 = _m[_targetRow][col];
        _m[_targetRow][col] = val1 * _c + val2 * _s;
        _m[_targetRow - 1][col] = val1* -_s + val2 *_c;
    }
}

void project(const MatrixValue * _u, const MatrixValue * _a, MatrixValue * _destination, u32 _size)
{
    MatrixValue multiplier = dotProduct(_u, _a, _size) / dotProduct(_u, _u, _size);
    multiplyVector(_u, _destination, _size, multiplier);
}

void multiplyVector(const MatrixValue * _vector, MatrixValue * _destination, u32 _size, MatrixValue _scalar)
{
    for (u32 i = 0; i < _size; i++)
    {
        _destination[i] = _scalar * _vector[i];
    }
}

void copyVector(const MatrixValue * _source, MatrixValue * _dest, u32 _size)
{
    for (u32 i = 0; i < _size; i++)
    {
        _dest[i] = _source[i];
    }
}

void subtractVectors(const MatrixValue* _left, const MatrixValue* _right, MatrixValue* _dest, u32 _size)
{
    for (int i = 0; i < _size; i++)
    {
        _dest[i] = _left[i] - _right[i];
    }
}

void normalizeVector(MatrixValue* _vector, u32 _size)
{
    MatrixValue len = sqrt(dotProduct(_vector, _vector, _size));
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
        index = alignTo4(index); // TODO - move this to BMPReader - this is bmp specific
    }

    return std::move(matrices);
}

ImageData matricesToImage(const std::vector<Matrix>& _matrices, PixelConvertion _convertion)
{
    _ASSERT(_matrices.size() == 3);

    const Matrix& r = _matrices[0];
    const Matrix& g = _matrices[1];
    const Matrix& b = _matrices[2];

    _ASSERT(r.getRows() == g.getRows() && g.getRows() == b.getRows());
    _ASSERT(r.getCols() == g.getCols() && g.getCols() == b.getCols());

    u16 rows = _matrices[0].getRows();
    u16 cols = _matrices[0].getCols();
    u32 pixelBufferSize = rows * cols * 3;

    u8* pixelBuffer = new u8[pixelBufferSize];

    u32 index = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            pixelBuffer[index++] = pixelConvert(r[i][j], _convertion);
            pixelBuffer[index++] = pixelConvert(g[i][j], _convertion);
            pixelBuffer[index++] = pixelConvert(b[i][j], _convertion);
        }
    }

    Blob pixels(pixelBuffer, pixelBufferSize);
    return ImageData(std::move(pixels), cols, rows);
}

std::vector<Matrix> QRAlgorithm(const Matrix & _matrix, u32 _iterations)
{
    std::vector<Matrix> qs;
    qs.reserve(_iterations);

    Matrix A = _matrix;
    for (int i = 0; i < _iterations; i++)
    {
        auto qr = QRGivensRotations(A);
        // auto qr = QRGramSchmidt(A);
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

    MatrixValue* temp = new MatrixValue[vectorLen];

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

std::vector<Matrix> QRGivensRotations(const Matrix & _matrix)
{
    Matrix m = _matrix;
    Matrix q = Matrix::identity(m.getRows(), m.getCols());
    for (int i = 0; i < m.getCols(); i++)
    {
        for (int j = m.getRows() - 1; j >= i + 1; j--)
        {
            if (abs(m[j][i]) < 0.00001)
            {
                m[j][i] = 0.f;
                continue;
            }
            auto cs = getGivensCoeffs(m[j][i], m[j - 1][i]);
            applyGivensRotation(m, cs.first, cs.second, j, i);
            applyGivensRotation(q, cs.first, cs.second, j, 0);
            if (abs(m[j][i]) < epsilon6)
            {
                m[j][i] = 0.f;
            };
        }
    }

    return std::vector<Matrix>{ q.transpose(), std::move(m) };
}

std::vector<Matrix> SVD(const Matrix & _matrix)
{
    if (_matrix.getRows() < _matrix.getCols())
    {
        auto svd = SVD(_matrix.transpose());
        std::swap(svd[0], svd[2]);
        return svd;
    }
    const Matrix& A = _matrix;

    std::vector<Matrix> qr;
    qr = QRAlgorithm(A.transpose() * A, 7);
    Matrix s = std::move(qr[0]);
    Matrix u = std::move(qr[1]);

    for (u16 i = 0; i < s.getRows(); i++)
    {
        for (int j = 0; j < s.getCols(); j++)
        {
            if (i != j)
            {
                s[i][j] = 0.f;
            }
            else
            {
                s[i][i] = sqrt(abs(s[i][i]));
            }
        }
    }

    // From : http://www.netlib.org/utk/people/JackDongarra/etemplates/node40.html
    // At * ui = si * vi
    Matrix v = (A*u).transpose();
    {
        for (u32 i = 0; i < v.getRows(); i++)
        {
            if (s[i][i] < epsilon6)
                continue;

            multiplyVector(v[i], v[i], v.getCols(), 1 / s[i][i]);
        }
    }
    std::vector<Matrix> svd;

    svd.push_back(v.transpose());
    svd.push_back(std::move(s));
    svd.push_back(std::move(u));

    return std::move(svd);
}

void draw(const ImageData& _image)
{
    u32 rows = _image.m_height;
    u32 cols = _image.m_width;
    HDC hdc = GetDC(GetConsoleWindow());

    u32 index = 0;
    for (int x = rows - 1; x >= 0; x--)
    {

        for (int y = 0; y < cols; ++y)
        {
            u8 r = _image.m_pixels.m_data[index++];
            u8 g = _image.m_pixels.m_data[index++];
            u8 b = _image.m_pixels.m_data[index++];
            SetPixel(hdc, y, x, RGB(b, g, r));
        }
    }
}

Matrix SVDCompress(const Matrix& _m, u32 _valuesToRemove)
{
    auto svd = SVD(_m);

    u32 lastIndex = svd[1].getRows() - 1;

    for (int i = 0; i < _valuesToRemove && lastIndex - i >= 0; i++)
    {
        svd[1][lastIndex - i][lastIndex - i] = 0;
    }
    auto result = svd[0] * svd[1] * svd[2].transpose();
    return result;
}

Matrix compress(const std::vector<Matrix>& _svd, u32 _valuesToRemove)
{
    Matrix sigma = _svd[1];
    u32 lastIndex = sigma.getRows() - 1;
    for (int i = 0; i < _valuesToRemove && lastIndex - i >= 0; i++)
    {
        sigma[lastIndex - i][lastIndex - i] = 0;
    }
    auto result = _svd[0] * sigma * _svd[2].transpose();
    return result;
}
