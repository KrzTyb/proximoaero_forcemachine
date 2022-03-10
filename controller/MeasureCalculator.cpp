#include "MeasureCalculator.hpp"

#include "config.h"

#include <cmath>
#include <functional>
#include <algorithm>
#include <numeric>

namespace
{

constexpr const auto MaxAdcValue = 4'096.0f;
constexpr const auto MaxVoltage = 5.0f;

#if BUILD_PC == 1
constexpr const auto MeasureFrequency = 10'000.0f;
#else
constexpr const auto MeasureFrequency = 8'000.0f;
#endif

constexpr const auto Dt = (1.0f/MeasureFrequency);

// Przyśpieszenie ziemskie
constexpr const float Gravity = 9.8105f;

constexpr const float ThresholdUp = 0.0f;
constexpr const float ThresholdDown = 0.0f;

constexpr const float InitialScale = 6.0f;

size_t findFirstAbove(const VoltageInTime& voltages, float threshold)
{
    size_t index = 0;

    for (const auto& voltage : voltages)
    {
        if (voltage.second > threshold)
        {
            return index;
        }
        index++;
    }
    return index;
}

size_t findEndTick(const VoltageInTime& voltages, float threshold, size_t startIndex)
{
    size_t index = startIndex;
    for (index = startIndex; index < voltages.size(); index++)
    {
        if (voltages.at(index).second < threshold)
        {
            return index;
        }
    }
    return index;
}

float calculateForce(float voltage)
{
    return (voltage - 0.5f) * 250.0;
}

std::pair<Displacement, Force> calculateDisplacement(const std::pair<TimeSec, Force>& forceInTime,
    float& forceSum, MeasureCalculator::Scale scale, MeasureCalculator::Height height)
{
    forceSum += forceInTime.second;

    auto sumScale = InitialScale + scale;

    auto first = Gravity * sqrtf((2.0f * height) / Gravity) * forceInTime.first;

    auto second = ((Gravity * powf(forceInTime.first, 2.0f)) / 2.0f);

    auto third = (powf(Dt, 2.0f) / sumScale) * forceSum;

    auto displacement = first + second - third;

    return std::make_pair(displacement, forceInTime.second);
}

}

MeasureCalculator::MeasureCalculator(std::vector<int>&& rawMeasures, Scale scale, Height height)
{
    TimeSec time = 0.0f;
    for (const auto& value : rawMeasures)
    {
        const float voltage = MaxVoltage * (static_cast<float>(value)/MaxAdcValue);
        m_voltages.emplace_back(std::make_pair(std::ref(time), std::ref(voltage)));
        time += Dt;
    }

    // Ucinanie pierwszego ticku zakłóceń (250ms)
    constexpr auto FirstSamplesToRemove = static_cast<size_t>(MeasureFrequency * 0.25f);
    m_voltages.erase(m_voltages.begin(), m_voltages.begin() + FirstSamplesToRemove);

    // Obliczanie średniej
    auto sum = std::accumulate(m_voltages.begin(), m_voltages.end(), float(0.0f), [](auto& accumulated, const auto& el) {
        return accumulated += el.second;
    });
    float mean = static_cast<float>(sum) / static_cast<float>(m_voltages.size());

    // Szukanie momentu uderzenia
    auto startTick = findFirstAbove(m_voltages, mean + ThresholdUp);
    auto endTick = findEndTick(m_voltages, mean - ThresholdDown, startTick);
    endTick += 100; // Add 100ms
    auto startImpact = (startTick >= m_voltages.size()) ? m_voltages.end() : (m_voltages.begin() + startTick);
    auto endImpact = (endTick >= m_voltages.size()) ? m_voltages.end() : (m_voltages.begin() + endTick);

    m_impactVoltages = VoltageInTime{startImpact, endImpact};

    // Rozpoczęcie liczenia czasu od 0
    float startTime = m_impactVoltages.at(0).first;
    std::transform(m_impactVoltages.begin(), m_impactVoltages.end(), m_impactVoltages.begin(),
        [startTime](const auto& element)
        {
            return std::make_pair(element.first - startTime, element.second);
        });

    // Obliczanie siły
    std::transform(m_impactVoltages.begin(), m_impactVoltages.end(), std::back_inserter(m_results),
        [](const auto& element)
        {
            return std::make_pair(element.first, calculateForce(element.second));
        });
    
    float forceSum = 0.0f;
    // Obliczanie przemieszczenia
    std::transform(m_results.begin(), m_results.end(), m_results.begin(),
        [&](const auto& element)
        {
            return calculateDisplacement(element, forceSum, scale, height);
        });
}