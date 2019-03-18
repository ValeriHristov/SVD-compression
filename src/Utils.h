#pragma once
#include <vector>
#include "Matrix.h"

struct ImageData;

inline u32 alignTo4(u32 _num)
{
    return (_num + 3) & ~3;
}

std::vector<Matrix> imageToMatrices(const ImageData& _image);
