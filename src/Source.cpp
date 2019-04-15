#include <iostream>
#include <string>
#include <fstream>

#include "Utils.h"
#include "Matrix.h"
#include "BMPReader.h"
#include "ScopedTimer.h"

#include <thread>

template<typename Func>
void runPerformanceTest(std::string _description, Func _f)
{
    ScopedTimer t(_description);

    _f();
}

int main()
{
    BMPReader reader;
    ImageData image = reader.ReadFile("../SVD-compression/data/6-4-balloon.bmp");
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