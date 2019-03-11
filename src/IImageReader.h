#pragma once
#include <utility>
#include "Types.h"

struct Blob
{
    Blob(u8* _data, u32 _size) : m_data(_data), m_size(_size) {}

    Blob(const Blob&) = delete;
    Blob& operator=(const Blob&) = delete;

    Blob(Blob&& _other) 
    {
        m_data = _other.m_data;
        m_size = _other.m_size;
        _other.m_data = nullptr;
    }
    Blob& operator=(Blob&& _other)
    {
        m_data = _other.m_data;
        m_size = _other.m_size;
        _other.m_data = nullptr;
        return *this;
    }

    ~Blob()
    {
        delete[] m_data;
    }
    u8* m_data = nullptr;
    u32 m_size = 0;
};

struct ImageData
{
    ImageData() = default;
    ImageData(Blob _pixels, u16 _width, u16 _height, u8 _bitsPerPixel = 24U) 
        : m_pixels(std::move(_pixels)), m_width(_width), m_height(_height), m_bitsPerPixel(_bitsPerPixel)
    {}

    ImageData(const ImageData&) = delete;
    ImageData& operator=(const ImageData&) = delete;

    ImageData(ImageData&&) = default;
    ImageData& operator=(ImageData&&) = default;
    ~ImageData() = default;

    Blob m_pixels;
    u16  m_width = 0U;
    u16  m_height = 0U;
    u8   m_bitsPerPixel = 24U;

};

class IImageReader
{
public:
    virtual ImageData ReadFile(const char* _fileName) const = 0;
};