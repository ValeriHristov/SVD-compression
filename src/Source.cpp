#include <iostream>
#include <string>
#include <fstream>
#include <random>
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
    auto image = reader.ReadFile("../SVD-compression/data/6-4-balloon.BMP");
    auto matrices = imageToMatrices(image);

    std::vector<Matrix> svd0 = SVD(matrices[0]);
    std::vector<Matrix> svd1 = SVD(matrices[1]);
    std::vector<Matrix> svd2 = SVD(matrices[2]);

    while (1)
    {
        u32 valueToRemove = 0;
        std::cin >> valueToRemove;
        auto compressed = std::vector<Matrix>{ compress(svd0,valueToRemove),
                                               compress(svd1,valueToRemove),
                                               compress(svd2,valueToRemove) 
                                             };

        std::cout << "Error: " << (matrices[0] - compressed[0]).norm() << '\n';
        std::cout << "Error: " << (matrices[1] - compressed[1]).norm() << '\n';
        std::cout << "Error: " << (matrices[2] - compressed[2]).norm() << '\n';
        auto img = matricesToImage(compressed, None);

        draw(img);
    }
}