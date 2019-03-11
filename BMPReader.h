#pragma once

#include "IImageReader.h"

class BMPReader : public IImageReader
{
public:
    ImageData ReadFile(const char * _fileName) const override;
};