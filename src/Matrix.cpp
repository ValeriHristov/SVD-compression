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

    i32 sum;

    for (u16 i = 0; i < m; i++)
    {
        for (u16 j = 0; j < p; j++)
        {
            sum = 0;
            for (u16 k = 0; k < n; k++)
            {
                i32 l = _left[i][k];
                i32 r = _right[k][j];
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

    i32 sum;

    for (u16 i = 0; i < m; i++)
    {
        for (u16 j = 0; j < p; j++)
        {
            sum = 0;
            for (u16 k = 0; k < n; k++)
            {
                i32 l = _left[i][k];
                i32 r = _right[j][k];
                sum += (l * r);
            }
            product[i][j] = sum;
        }
    }

    return std::move(product);
}
