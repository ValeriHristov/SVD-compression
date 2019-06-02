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

template< class T>
T max(T a, T b)
{
    if (a > b) return a;
    return b;
}

int main()
{
    BMPReader reader;
    auto image = reader.ReadFile("../SVD-compression/data/marbles.BMP");
    auto matrices = imageToMatrices(image);

    std::vector<Matrix> svd0 = SVD(matrices[0]);
    std::vector<Matrix> svd1 = SVD(matrices[1]);
    std::vector<Matrix> svd2 = SVD(matrices[2]);

    while (1)
    {
        u32 valuesToRemove = 0;
        std::cin >> valuesToRemove;
        auto compressed = std::vector<Matrix>{ compress(svd0,valuesToRemove),
                                               compress(svd1,valuesToRemove),
                                               compress(svd2,valuesToRemove)
                                             };

        std::cout << "Error: " << (matrices[0] - compressed[0]).norm() << '\n';
        std::cout << "Error: " << (matrices[1] - compressed[1]).norm() << '\n';
        std::cout << "Error: " << (matrices[2] - compressed[2]).norm() << '\n';

        int compressedSize = max(0u,(svd0[0].getCols() - valuesToRemove)*svd0[0].getRows()) + max(0u,(svd0[2].getCols() - valuesToRemove)*svd0[2].getRows());
        int originalSize = matrices[0].getCols() * matrices[0].getRows();
        std::cout << "                                                 Memory used:" << (compressedSize*1.0 / originalSize) * 100 << "%\n";
        auto img = matricesToImage(compressed, None);

        draw(img);
    }
}