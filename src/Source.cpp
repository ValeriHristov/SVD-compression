#include <iostream>
#include <string>
#include <fstream>

#include "Utils.h"
#include "Matrix.h"
#include "BMPReader.h"

int main()
{
    BMPReader reader;
    ImageData image = reader.ReadFile("../SVD-compression/data/FLAG_B24.BMP");
    std::vector<Matrix> matrices = imageToMatrices(image);

    for (u32 i = 0; i < 3; i++)
    {
        std::string file = "matrix";
        file = file + std::to_string(i) + ".txt";

        std::ofstream os;
        os.open(file,std::ofstream::trunc);

        matrices[i].exportForWolframMathematica(os);
    }

    return 0;
}