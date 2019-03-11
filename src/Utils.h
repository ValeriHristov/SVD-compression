#pragma once
#include <vector>
#include "Matrix.h"

struct ImageData;

std::vector<Matrix> imageToMatrices(const ImageData& _image);
