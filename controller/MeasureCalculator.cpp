#include "MeasureCalculator.hpp"

#include <cmath>
#include <functional>

constexpr const auto MaxAdcValue = 4'096.0f;
constexpr const auto MeasureFrequency = 8'000;
constexpr const auto MaxVoltage = 3.3f;
constexpr const auto SampleTimeSec = (1.0f/static_cast<float>(MeasureFrequency));

MeasureCalculator::MeasureCalculator(std::vector<int>&& rawMeasures)
{
    TimeSec time = 0.0f;
    for (const auto& value : rawMeasures)
    {
        const float voltage = MaxVoltage * (static_cast<float>(value)/MaxAdcValue);
        m_voltages.emplace_back(std::make_pair(std::ref(time), std::ref(voltage)));
        time += SampleTimeSec;
    }
}