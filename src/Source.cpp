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
    int rows = 1000;
    int cols = 1000;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);

    Matrix a(rows, cols);
    int counter = 1;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            a[i][j] = dis(gen);
        }
    }

    auto svd = SVD(a);

    Matrix res = svd[0] * svd[1] * svd[2].transpose();

    return 0;
}