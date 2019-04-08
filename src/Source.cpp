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
  //  a[0][0] = 12;
  //  a[0][1] = -51;
  //  a[0][2] = 4;
  // 
  //  a[1][0] = -51;
  //  a[1][1] = 167;
  //  a[1][2] = -68;
  // 
  //  a[2][0] = 4;
  //  a[2][1] = -68;
  //  a[2][2] = -41;
  //
  //  auto res = QRAlgorithm(a, 15);
  //  res[0].print(); // values on diagonal
  //  res[1].print(); // vectors by rows


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
      u16 size = 1024;
      float multiplier = 1.;
      Matrix x(multiplier*size, size);
 //     Matrix y(size, multiplier*size);
      Matrix transpose, standard, transposeSIMD, threaded;
      {
          for (int i = 0; i < multiplier * size; i++)
          {
              for (int j = 0; j < size; j++)
              {
                  x[i][j] =  rand() & 63;
                //  y[j][i] = 1;// rand() & 63;
              }
          }
      }

      std::vector<Matrix> v;

      runPerformanceTest("SVD", [&]()
      {
          auto sim = x * x.transpose();
          v = QRAlgorithm(sim, 15);
      });

    //
    //   runPerformanceTest("gram", [&]()
    //   {
    //       QRGramSchmidt(x);
    //
    //   });
    //
    //   u32 repetitionsCount = 1;
    //   runPerformanceTest("Standard", [&]()
    //   {
    //       for (int i = 0; i < repetitionsCount; i++)
    //       {
    //           standard = mulSlow(x, y);
    //       }
    //   });
    //
    //   runPerformanceTest("With transposition", [&]() 
    //   {
    //       Matrix tmp;
    //       for (size_t i = 0; i < repetitionsCount; i++)
    //       {
    //           {
    //               //   ScopedTimer b("Transposition");
    //               tmp = y.transpose();
    //
    //           }
    //           transpose = mulTransposed(x, tmp);
    //       }
    //   });
    //       
    //  runPerformanceTest("With transposition and SIMD", [&]()
    // {
    //     Matrix tmp;
    //     for (size_t i = 0; i < repetitionsCount; i++)
    //     {
    //         {
    //             //   ScopedTimer b("Transposition");
    //             tmp = y.transpose();
    //         }
    //         transposeSIMD = mulTransposedSIMD(x, tmp);
    //     }
    // });
    // //
    // //
    // //  runPerformanceTest("Threaded with 8", [&]()
    // //  {
    // //      mul(x, y, 8);
    // //  });
    // //
    //   runPerformanceTest("Threaded with 4", [&]()
    //   {
    //       threaded = mul(x, y, 4);
    //   });
    //       
    //  std::cout <<(threaded == transposeSIMD);
    //
    // //  runPerformanceTest("Threaded with 3", [&]()
    // //  {
    // //      mul(x, y, 3);
    // //  });
    // //
    // //  runPerformanceTest("Threaded with 2", [&]()
    // //  {
    // //      mul(x, y, 2);
    // //  });
    // //
    // //  runPerformanceTest("Threaded with 1", [&]()
    // //  {
    // //      mul(x, y, 1);
    // //  });

    return 0;
}