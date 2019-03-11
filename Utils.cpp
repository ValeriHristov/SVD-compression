#include "Utils.h"
#include "IImageReader.h"

std::vector<Matrix> imageToMatrices(const ImageData & _image)
{
    //TODO support different pixel formats
    //now, assume 24bit RGB -> 3 matrices

    const u8 matrixCount = 3u;

    std::vector<Matrix> matrices(matrixCount);
    std::fill(matrices.begin(), matrices.end(), Matrix(_image.m_height, _image.m_width));

    u8* data = static_cast<u8*>(_image.m_pixels.m_data);

    for (u32 i = 0u; i < _image.m_height; i++)
    {
        for (u32 j = 0u; j < _image.m_width; j++)
        {
            for (u32 m = 0u; m < matrixCount; m++)
            {
                matrices[m][i][j] = static_cast<MatrixValue>(data[i * _image.m_width + j + m]);
            }
        }
    }



    return std::move(matrices);
}
