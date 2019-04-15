#include <utility>
#include <thread>
#include <vector>
#include <immintrin.h>

#include "Matrix.h"
u32 alignTo(u32 _num, u32 _alignment)
{
    if (_num % _alignment == 0) return _num;
    return _num + (_alignment - _num % _alignment);
}

void Matrix::clear()
{
    delete[] m_values;
}

void  Matrix::alloc(u32 _count, u32 _alignment)
{
    m_values = new MatrixValue[_count];
}

Matrix::Matrix(u16 _rows, u16 _cols) : m_rows(_rows), m_cols(_cols)
{
    alloc(_rows*_cols, 64);
}

Matrix::Matrix(const Matrix & _other)
{
    copy(_other);
}

Matrix::Matrix(Matrix && _other)
{
    move(std::move(_other));
}

Matrix & Matrix::operator=(const Matrix & _other)
{
    if (this != &_other)
    {
        clear();
        copy(_other);
    }
    return *this;
}

Matrix & Matrix::operator=(Matrix && _other)
{
    if (this != &_other)
    {
        clear();
        move(std::move(_other));
    }
    return *this;
}

Matrix::~Matrix()
{
    clear();
}

Matrix Matrix::transpose() const
{
    Matrix result(m_cols, m_rows);

    for (int i = 0; i < m_rows; i++)
    {
        for (int j = 0; j < m_cols; j++)
        {
            result[j][i] = (*this)[i][j];
        }
    }
    return std::move(result);
}

void Matrix::print(std::ostream & _os) const
{
    for (u32 i = 0; i < m_rows; i++)
    {
        for (u32 j = 0; j < m_cols; j++)
        {
            _os << (*this)[i][j] << ' ';
        }
        _os << '\n';
    }
}

void Matrix::exportForWolframMathematica(std::ostream & _os)
{
    // by default .bmp keeps the rows in down-up order, so reverse them
    // TODO support the other case
    for (u32 i = 0; i < m_rows; i++)
    {
        for (u32 j = 0; j < m_cols - 1; j++)
        {
            _os << ((*this)[m_rows - i - 1][j]) / 255. << ' ';
        }
        _os << ((*this)[0][m_cols - 1]) / 255. << '\n';
    }
}

void Matrix::copy(const Matrix & _other)
{
    m_rows = _other.m_rows;
    m_cols = _other.m_cols;
    u32 size = m_rows * m_cols;
    alloc(size, 64);
    for (u32 i = 0u; i < size; i++)
    {
        m_values[i] = _other.m_values[i];
    }
}

void Matrix::move(Matrix && _other)
{
    m_rows = _other.m_rows;
    m_cols = _other.m_cols;
    m_values = _other.m_values;
    _other.m_values = nullptr;
}

float dotProduct(const float * _left, const float * _right, u32 _size)
{
    __m128 sum;
    float tsum = 0;

    tsum = 0.f;
    sum = _mm_setzero_ps();

    if (_size >= 4)
    {
        for (u16 k = 0; k < _size - 3; k += 4)
        {
            __m128 l = _mm_loadu_ps(_left + k);
            __m128 r = _mm_loadu_ps(_right + k);
            sum = _mm_add_ps(_mm_mul_ps(l, r), sum);
        }
    }
    alignas(16) float s[4];

    _mm_store_ps(s, sum);
    tsum = s[0] + s[1] + s[2] + s[3];

    for (int k = _size & ~3; k < _size; k++)
    {
        float l = _left[k];
        float r = _right[k];
        tsum += (l*r);
    }

    return tsum;
}

Matrix operator*(const Matrix & _left, const Matrix & _right)
{
    return mul(_left, _right);
}

Matrix mulSlow(const Matrix & _left, const Matrix & _right)
{
    u16 m = _left.getRows();
    u16 n = _left.getCols();
    u16 p = _right.getCols();

    if (_left.getCols() != _right.getRows())
    {
        std::cerr << "Invalid multiplication!\n";
        return Matrix();
    }

    Matrix product(m, p);

    float sum;
    for (u16 i = 0; i < m; i++)
    {
        for (u16 j = 0; j < p; j++)
        {
            sum = 0;
            for (u16 k = 0; k < n; k++)
            {
                float l = _left[i][k];
                float r = _right[k][j];
                sum += (l * r);
            }
            product[i][j] = sum;
        }
    }
    return std::move(product);
}

Matrix mul(const Matrix & _left, const Matrix & _right, u32 _threadCount)
{
    Matrix transposed = _right.transpose();

    u32 matrixRows = transposed.getRows();

    if (_threadCount > matrixRows)
    {
        _threadCount = matrixRows;
    }

    return mulThreaded(_left, transposed, _threadCount);
}

Matrix mulTransposed(const Matrix & _left, const Matrix & _right)
{
    u16 m = _left.getRows();
    u16 n = _left.getCols();
    u16 p = _right.getRows();
    if (_left.getCols() != _right.getCols())
    {
        std::cerr << "Invalid multiplication!\n";
        return Matrix();
    }

    Matrix product(m, p);
    float sum;
    for (u16 i = 0; i < m; i++)
    {
        for (u16 j = 0; j < p; j++)
        {
            sum = 0;
            for (u16 k = 0; k < n; k++)
            {
                float l = _left[i][k];
                float r = _right[j][k];
                sum += (l * r);
            }
            product[i][j] = sum;
        }
    }
    return std::move(product);
}

void mulRowsChunk(u32 _startRow, u32 _rowsToProcess, const Matrix& _left, const Matrix& _right, Matrix& _result)
{
    u16 n = _left.getCols();
    u16 p = _right.getRows();
    for (u16 i = _startRow; i < _startRow + _rowsToProcess; i++)
    {
        for (u16 j = 0; j < p; j++)
        {
            _result[i][j] = dotProduct(_left[i], _right[j], n);
        }
    }
}

Matrix mulThreaded(const Matrix & _left, const Matrix & _right, u32 _numThreads)
{
    if (_left.getCols() != _right.getCols())
    {
        std::cerr << "Invalid multiplication!\n";
        return Matrix();
    }
    u16 m = alignTo(_left.getRows(), _numThreads);
    u16 rowsPerThread = m / _numThreads;
    i16 lastThreadRows = rowsPerThread - (m - _left.getRows());
    Matrix product(_left.getRows(), _right.getRows());

    std::vector<std::thread>threads(_numThreads);

    for (int i = 0; i < _numThreads - 1; i++)
    {
        threads[i] = std::thread(mulRowsChunk, i * rowsPerThread, rowsPerThread, std::ref(_left), std::ref(_right), std::ref(product));
    }

    threads[_numThreads - 1] = std::thread(mulRowsChunk, (_numThreads - 1) * rowsPerThread, lastThreadRows, std::ref(_left), std::ref(_right), std::ref(product));

    for (size_t i = 0; i < _numThreads; i++)
    {
        if (threads[i].joinable())
            threads[i].join();
    }
    return product;
}

Matrix mulTransposedSIMD(const Matrix & _left, const Matrix & _right)
{
    u16 m = _left.getRows();
    u16 p = _right.getRows();
    Matrix product(m, p);

    mulRowsChunk(0, m, _left, _right, product);
    return std::move(product);
}
