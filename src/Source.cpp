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
    //  Matrix a(3, 3);
    //  a[0][0] = 0;
    //  a[0][1] = 8;
    //  a[0][2] = 4;
    // 
    //  a[1][0] = -51;
    //  a[1][1] = -17;
    //  a[1][2] = -68;
    // 
    //  a[2][0] = 0;
    //  a[2][1] = -68;
    //  a[2][2] = -41;

         //  for (int i = 0; i < 10; i++)
         //  {
         //      _sleep(1000);
         //      ScopedTimer t("Decompose");
         //      BMPReader reader;
         //      ImageData image = reader.ReadFile("../SVD-compression/data/MARBLES.bmp");
         //      std::vector<Matrix> matrices = imageToMatrices(image);
         // 
         //      std::vector<Matrix> multiplied(3);
         // 
         //      for (u32 i = 0; i < 3; i++)
         //      {
         //          multiplied.emplace_back(mulTransposed(matrices[i], matrices[i]));
         //          // std::string file = "matrix";
         //          // file = file + std::to_string(i) + ".txt";
         //          //
         //          // std::ofstream os;
         //          // os.open(file,std::ofstream::trunc);
         //          //
         //          // matrices[i].exportForWolframMathematica(os);
         //      }
         //  }
    u16 rows = 3;
    u16 cols = 4;
    Matrix x(rows, cols);
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                x[i][j] = rand() % 63;
            }
        }
    }
    // x = a;
    x.print();
    std::cout << "================================================\n\n";

    auto svd = SVD(x);


    ((svd[0] * svd[1]) * svd[2].transpose()).print();


    return 0;
}