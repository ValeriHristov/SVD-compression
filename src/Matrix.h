#pragma once
#include <iostream>
#include "Types.h"

typedef u8 MatrixValue;

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

    void print(std::ostream& _os = std::cout);

    // export format for WolframMathematica
    // import by using ReadList[ PATH/file, Number, RecordLists -> True]
    void exportForWolframMathematica(std::ostream& _os = std::cout);

private:
    void copy(const Matrix& _other);
    void move(Matrix&& _other);

    MatrixValue* m_values = nullptr;
    u16 m_rows = 0;
    u16 m_cols = 0;
};

Matrix operator*(const Matrix& _left, const Matrix& _right);

Matrix mul(const Matrix& _left, const Matrix& _right);

// multiplies a*bT
Matrix mulTransposed(const Matrix& _left, const Matrix& _right);