#include "GPIOController.hpp"

#include "config.h"


#if BUILD_ARM == 1

namespace
{

static auto chip = ::gpiod::chip{"gpiochip0"};

constexpr auto REQUEST_CONSUMER = "ForceMachine";

}

GPIO::GPIO(uint8_t pinNumber, GPIO_TYPE type, bool defaultState, OUTPUT_TYPE outType)
{
    m_line = chip.get_line(pinNumber);

    m_lineRequest = ::gpiod::line_request{};
    m_lineRequest.consumer = REQUEST_CONSUMER;
    m_lineRequest.request_type = static_cast<int>(type);

    if (outType == OUTPUT_TYPE::OPEN_DRAIN)
    {
        m_lineRequest.flags = ::gpiod::line_request::FLAG_OPEN_DRAIN;
    }

    m_line.request(m_lineRequest, defaultState);
}

GPIO::~GPIO()
{
    m_line.release();
}

bool GPIO::read()
{
    return m_line.get_value();
}

void GPIO::write(bool state)
{
    m_line.set_value(state);
}

bool GPIO::waitToEvent(const ::std::chrono::nanoseconds& timeout)
{
    return m_line.event_wait(timeout);
}

GPIO_EVENT_TYPE GPIO::getEventType()
{
    if (m_line.event_read().event_type == ::gpiod::line_event::RISING_EDGE)
    {
        return GPIO_EVENT_TYPE::RISING_EDGE;
    }
    return GPIO_EVENT_TYPE::FALLING_EDGE;
}

#else

#include <QThread>

GPIO::GPIO(uint8_t pinNumber, GPIO_TYPE type, bool defaultState, OUTPUT_TYPE outType)
{

}

GPIO::~GPIO()
{
}

bool GPIO::read()
{
    return false;
}

void GPIO::write(bool state)
{

}

bool GPIO::waitToEvent(const ::std::chrono::nanoseconds& timeout)
{
    QThread::usleep(timeout.count() / 1000.0);
    return false;
}

GPIO_EVENT_TYPE GPIO::getEventType()
{
    return GPIO_EVENT_TYPE::UNKNOWN;
}


#endif