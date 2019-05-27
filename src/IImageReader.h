#pragma once

#include "Types.h"

class IImageReader
{
public:
    virtual ImageData ReadFile(const char* _fileName) const = 0;
};