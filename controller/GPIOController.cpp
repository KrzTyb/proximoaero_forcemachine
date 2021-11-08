#include "GPIOController.hpp"


namespace
{

static auto chip = ::gpiod::chip{"gpiochip0"};

constexpr auto REQUEST_CONSUMER = "ForceMachine";

}

GPIO::GPIO(uint8_t pinNumber, GPIO_TYPE type, bool defaultState)
{
    m_line = chip.get_line(pinNumber);

    m_lineRequest = ::gpiod::line_request{REQUEST_CONSUMER, static_cast<int>(type), 0};

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