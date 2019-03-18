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
        copy(_other);
    }
    return *this;
}

Matrix & Matrix::operator=(Matrix && _other)
{
    move(std::move(_other));
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
