#include "MeasureCalculator.hpp"

#include "config.h"

#include <cmath>
#include <functional>
#include <algorithm>
#include <numeric>

namespace
{

constexpr const auto MaxAdcValue = 4'096.0;
constexpr const auto MaxVoltage = 5.0;

#if BUILD_PC == 1
constexpr const auto MeasureFrequency = 10'000.0;
#else
constexpr const auto MeasureFrequency = 8'000.0;
#endif

constexpr const auto Dt = (1.0/MeasureFrequency);

// Przyśpieszenie ziemskie
constexpr const double Gravity = 9.8105;

constexpr const double ThresholdUp = 0.0;
constexpr const double ThresholdDown = 0.0;

constexpr const double InitialScale = 6.0;

size_t findFirstAbove(const VoltageInTime& voltages, double threshold)
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

size_t findEndTick(const VoltageInTime& voltages, double threshold, size_t startIndex)
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

double calculateForce(double voltage)
{
    return (voltage - 0.5) * 250.0;
}

std::pair<Displacement, Force> calculateDisplacement(const std::pair<TimeSec, Force>& forceInTime,
    double& forceSum, double& forceSumSum, MeasureCalculator::Scale scale, MeasureCalculator::Height height)
{
    forceSum += forceInTime.second;
    forceSumSum += forceSum;

    auto sumScale = InitialScale + scale;

    auto first = Gravity * sqrt((2.0 * height) / Gravity) * forceInTime.first;

    auto second = ((Gravity * pow(forceInTime.first, 2.0)) / 2.0);

    auto third = (pow(Dt, 2.0) / sumScale) * forceSumSum;

    auto displacement = first + second - third;

    return std::make_pair(displacement, forceInTime.second);
}

}

MeasureCalculator::MeasureCalculator(std::vector<int>&& rawMeasures, Scale scale, Height height)
{
    TimeSec time = 0.0;
    for (const auto& value : rawMeasures)
    {
        const double voltage = MaxVoltage * (static_cast<double>(value)/MaxAdcValue);
        m_voltages.emplace_back(std::make_pair(std::ref(time), std::ref(voltage)));
        time += Dt;
    }

    // Ucinanie pierwszego ticku zakłóceń (250ms)
    constexpr auto FirstSamplesToRemove = static_cast<size_t>(MeasureFrequency * 0.25);
    m_voltages.erase(m_voltages.begin(), m_voltages.begin() + FirstSamplesToRemove);

    // Obliczanie średniej
    auto sum = std::accumulate(m_voltages.begin(), m_voltages.end(), double(0.0), [](auto& accumulated, const auto& el) {
        return accumulated += el.second;
    });
    double mean = static_cast<double>(sum) / static_cast<double>(m_voltages.size());

    // Szukanie momentu uderzenia
    auto startTick = findFirstAbove(m_voltages, mean + ThresholdUp);
    auto endTick = findEndTick(m_voltages, mean - ThresholdDown, startTick);
    endTick += 100; // Add 100ms
    auto startImpact = (startTick >= m_voltages.size()) ? m_voltages.end() : (m_voltages.begin() + startTick);
    auto endImpact = (endTick >= m_voltages.size()) ? m_voltages.end() : (m_voltages.begin() + endTick);

    m_impactVoltages = VoltageInTime{startImpact, endImpact};

    // Rozpoczęcie liczenia czasu od 0
    double startTime = m_impactVoltages.at(0).first;
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
    
    double forceSum = 0.0;
    double forceSumSum = 0.0;
    // Obliczanie przemieszczenia
    std::transform(m_results.begin(), m_results.end(), m_results.begin(),
        [&](const auto& element)
        {
            return calculateDisplacement(element, forceSum, forceSumSum, scale, height);
        });
}