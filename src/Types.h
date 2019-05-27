#pragma once
#include <Stdint.h>
#include <utility>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

constexpr float epsilon3 = 0.001f;
constexpr float epsilon6 = 0.000001f;

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
        delete[] m_data;

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