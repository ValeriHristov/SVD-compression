#include <utility>

#include "Matrix.h"

Matrix::Matrix(u16 _rows, u16 _cols) : m_rows(_rows), m_cols(_cols)
{
    m_values = new MatrixValue[_rows*_cols];
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
        delete[] m_values;
        copy(_other);
    }
    return *this;
}

Matrix & Matrix::operator=(Matrix && _other)
{
    if (this != &_other)
    {
        delete[] m_values;
        move(std::move(_other));
    }
    return *this;
}

Matrix::~Matrix()
{
    delete[] m_values;
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

void Matrix::print(std::ostream & _os)
{
    for (u32 i = 0; i < m_rows; i++)
    {
        for (u32 j = 0; j < m_cols; j++)
        {
            _os << (u32)(*this)[i][j] << ' ';
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
    m_values = new MatrixValue[size];
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

Matrix operator*(const Matrix & _left, const Matrix & _right)
{
    return mul(_left, _right);
}

Matrix mul(const Matrix & _left, const Matrix & _right)
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


Matrix mulTransposedSIMD(const Matrix & _left, const Matrix & _right)
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
    __m128 sum;
    float tsum = 0;

    for (u16 i = 0; i < m; i++)
    {
        for (u16 j = 0; j < p; j++)
        {
            tsum = 0.f;
            sum = _mm_setzero_ps();

            const float* lp = _left[i];
            const float* rp = _right[j];

            for (u16 k = 0; k < n - 3; k += 4)
            {
                __m128 l = _mm_loadu_ps(lp + k);
                __m128 r = _mm_loadu_ps(rp + k);
                sum = _mm_add_ps(_mm_mul_ps(l, r), sum);
            }

            alignas(16) float s[4];

            _mm_store_ps(s, sum);
            tsum = s[0] + s[1] + s[2] + s[3];

            for (int k = n & ~3; k < n; k++)
            {
                float l = _left[i][k];
                float r = _right[j][k];
                tsum += (l*r);
            }
            product[i][j] = tsum;
        }
    }
    return std::move(product);
}
