#pragma once

#include <stdint.h>
#include <chrono>
#include <string>

#include "config.h"

#if BUILD_ARM

#include <gpiod.hpp>

#endif

enum class GPIO_TYPE
{
    INPUT = 2, // Read GPIO
    OUTPUT, // Write GPIO
    FALLING, // Listen event falling edge
    RISING, // Listen event rising edge
    BOTH_EDGES // Listen event both edges
};

enum class OUTPUT_TYPE
{
    NORMAL,
    OPEN_DRAIN
};

enum class GPIO_EVENT_TYPE
{
    RISING_EDGE,
    FALLING_EDGE,
    UNKNOWN
};

class GPIO
{
public:
    GPIO(uint8_t pinNumber, GPIO_TYPE type, bool defaultState = 0, OUTPUT_TYPE outType = OUTPUT_TYPE::NORMAL);
    ~GPIO();

    bool read();
    void write(bool state);

    bool waitToEvent(const ::std::chrono::nanoseconds& timeout);
    GPIO_EVENT_TYPE getEventType();
    

    std::string getName()
    {
#if BUILD_ARM
        return m_line.name();
#else
        return "UNKNOWN";
#endif
    }

private:
#if BUILD_ARM
    ::gpiod::line           m_line;
    ::gpiod::line_request   m_lineRequest;
#endif
};
