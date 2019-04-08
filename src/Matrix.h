#pragma once
#include <iostream>
#include "Types.h"

typedef float MatrixValue;

float dotProduct(const float* _l, const float* _r, u32 _size);

class Matrix
{
public:
    Matrix() = default;
    Matrix(u16 _rows, u16 _cols);
    Matrix(const Matrix& _other);
    Matrix(Matrix&& _other);
    Matrix& operator=(const Matrix& _other);
    Matrix& operator=(Matrix&& _other);
    ~Matrix();

    u16 getRows() const { return m_rows; }
    u16 getCols() const { return m_cols; }

    inline MatrixValue* operator[](u16 _row) { return m_values + m_cols * _row; };
    inline const MatrixValue* operator[](u16 _row) const { return m_values + m_cols * _row; };

    bool operator==(const Matrix& _other)
    {
        if (m_rows != _other.m_rows || m_cols != _other.m_cols)
            return false;

        for (int i = 0; i < m_rows; i++)
        {
            for (int j = 0; j < m_cols; j++)
            {
                if ((*this)[i][j] != _other[i][j])
                    return false;
            }
        }
        return true;
    }

    Matrix transpose() const;

    void print(std::ostream& _os = std::cout);

    // export format for WolframMathematica
    // import by using ReadList[ PATH/file, Number, RecordLists -> True]
    void exportForWolframMathematica(std::ostream& _os = std::cout);

private:
    void copy(const Matrix& _other);
    void move(Matrix&& _other);
    void clear();
    void alloc(u32 _count, u32 _alignment);

    MatrixValue* m_values = nullptr;
    u16 m_rows = 0;
    u16 m_cols = 0;
};

Matrix operator*(const Matrix& _left, const Matrix& _right);

Matrix mulSlow(const Matrix& _left, const Matrix& _right);

// fastest multiplication using multithreading
Matrix mul(const Matrix & _left, const Matrix & _right, u32 _threadCount = 4);

// multiplies a*bT
Matrix mulTransposed(const Matrix& _left, const Matrix& _right);

// multiplies a*bT
Matrix mulTransposedSIMD(const Matrix & _left, const Matrix & _right);

Matrix mulThreaded(const Matrix & _left, const Matrix & _right, u32 _numThreads);