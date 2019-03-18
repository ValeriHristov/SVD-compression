#include <stdio.h>
#include <cmath>
#include "BMPReader.h"
#include "Utils.h"

ImageData BMPReader::ReadFile(const char * _fileName) const
{
    FILE* f = fopen(_fileName, "rb");
    u8 info[54];
    fread(info, sizeof(u8), 54, f); // read the 54-byte header

    i32 width = *(i32*)&info[18];
    i32 height = *(i32*)&info[22];

    // TODO support negative height sign 
    i32 heightSign = 1;
    if (height < 0) {
        heightSign = -1;
    }


    // TODO support different pixel formats
    u8 pixelSizeInBits = 24;

    u32 paddedRowLen = alignTo4(width * 3);


    u32 dataSize = paddedRowLen * height;

    u8* data = new u8[dataSize];
    fread(data, sizeof(u8), dataSize, f);
    fclose(f);

    Blob imageBlob(data, dataSize);
    return ImageData(std::move(imageBlob), width, height, pixelSizeInBits);
}
