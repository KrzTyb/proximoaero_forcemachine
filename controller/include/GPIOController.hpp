#pragma once

#include <stdint.h>
#include <chrono>

#include <gpiod.hpp>


enum class GPIO_TYPE
{
    INPUT = 2, // Read GPIO
    OUTPUT, // Write GPIO
    FALLING, // Listen event falling edge
    RISING, // Listen event rising edge
    BOTH_EDGES // Listen event both edges
};

enum class GPIO_EVENT_TYPE
{
    RISING_EDGE,
    FALLING_EDGE
};

class GPIO
{
public:
    GPIO(uint8_t pinNumber, GPIO_TYPE type, bool defaultState = 0);
    ~GPIO();

    bool read();
    void write(bool state);

    bool waitToEvent(const ::std::chrono::nanoseconds& timeout);
    GPIO_EVENT_TYPE getEventType();
    

    std::string getName() { return m_line.name(); }

private:

    ::gpiod::line           m_line;
    ::gpiod::line_request   m_lineRequest;
};
