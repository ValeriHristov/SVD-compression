#pragma once
#include <iostream>
#include <chrono>
#include <string>

class ScopedTimer
{
public:
    ScopedTimer(std::string _timerName) : m_name(_timerName)
    {
        m_start = std::chrono::steady_clock::now();
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

    ~ScopedTimer()
    {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        print(end);
    }

private:

    void print(const std::chrono::steady_clock::time_point& _end)
    {
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(_end - m_start).count();
        if (durationMs > 0)
        {
            std::cout << m_name << " took " << durationMs<< " miliseconds\n";
        }
        else
        {
            auto durationMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>(_end - m_start).count();
            std::cout << m_name << " took " << durationMicroSeconds << " microseconds\n";
        }
    }

    std::string m_name;
    std::chrono::steady_clock::time_point m_start;
};