#include <iostream>
#include "Utils.h"
#include "Matrix.h"
#include "BMPReader.h"

int main()
{
    BMPReader reader;
    ImageData image = reader.ReadFile("data/FLAG_B24.BMP");
    std::vector<Matrix> matrices = imageToMatrices(image);

    return 0;
}