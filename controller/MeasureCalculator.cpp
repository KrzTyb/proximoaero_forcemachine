#include "MeasureCalculator.hpp"

#include "config.h"

#include <cmath>
#include <functional>
#include <algorithm>
#include <numeric>
#include <tuple>

#include <QDebug>

namespace
{

constexpr const auto MaxAdcValue = 4'096.0;
constexpr const auto MaxVoltage = 3.3;

#if BUILD_PC == 1
constexpr const auto MeasureFrequency = 8'000.0;
#else
constexpr const auto MeasureFrequency = 8'000.0;
#endif

constexpr const auto Dt = (1.0/MeasureFrequency);

// Przyśpieszenie ziemskie
constexpr const double Gravity = 9.8105;

constexpr const double ThresholdUp = 0.0;
constexpr const double ThresholdDown = 0.0;

constexpr const double InitialScale = 6.0;

constexpr const auto MovingFilterLength = 8;

constexpr auto AddToEndTickS = 0.02;
constexpr auto AddToEndTickSamples = static_cast<int>(MeasureFrequency * AddToEndTickS);

void movingFilter(VoltageInTime& voltages)
{
    if (voltages.size() < (MovingFilterLength + 2))
    {
        return;
    }
    // Tymczasowa zmienna która przechowuje wartość w ostatnio sumowanym punkcie
    double lastValue = 0.0;

    // Suma dla zerowej próbki
    double sumValue = voltages.at(0).second;
    lastValue = sumValue;

    for (auto i = 1; i < MovingFilterLength; i++)
    {
        sumValue += voltages.at(i).second;
    }

    auto firstValue = voltages.at(0);
    firstValue.second = sumValue / MovingFilterLength;
    voltages[0] = firstValue;

    size_t i = 1;
    while (i < (voltages.size() - MovingFilterLength))
    {
        sumValue -= lastValue;
        sumValue += voltages.at(i + MovingFilterLength - 1).second;

        auto value = voltages.at(i);
        value.second = sumValue / MovingFilterLength;
        voltages[i] = value;
        i++;
        lastValue = value.second;
    }

    voltages.resize(voltages.size() - MovingFilterLength);
}

size_t findStartTick(const VoltageInTime& voltages, size_t maxIndex, double mean)
{
    size_t index = maxIndex;

    for (size_t i = maxIndex; i >= 0; i--)
    {
        if (voltages.at(i).second <= mean)
        {
            index = i;
            break;
        }
    }

    return index;
}

size_t findEndTick(const VoltageInTime& voltages, size_t maxIndex, double mean)
{
    size_t index = maxIndex;

    for (size_t i = maxIndex; i < voltages.size(); i++)
    {
        if (voltages.at(i).second <= mean)
        {
            index = i;
            break;
        }
    }

    return index;
}

size_t findMaxIndex(const VoltageInTime& voltages)
{
    auto maxValue = voltages.at(0).second;
    size_t index = 0;

    for (size_t i = 0; i < voltages.size(); i++)
    {
        if (voltages.at(i).second > maxValue)
        {
            maxValue = voltages.at(i).second;
            index = i;
        }
    }

    return index;
}

using StartTick = size_t;
using MaxIndex = size_t;
using EndTick = size_t;
std::tuple<StartTick, MaxIndex, EndTick> findStartEnd(const VoltageInTime& voltages, double mean)
{
    auto maxIndex = findMaxIndex(voltages);
    auto startTick = findStartTick(voltages, maxIndex, mean);
    auto endTick = findEndTick(voltages, maxIndex, mean);
    return {startTick, maxIndex, endTick};
}

double calculateForce(double voltage)
{
    return (voltage) * 3030.3;
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
    constexpr auto VoltagePerStep = MaxVoltage / MaxAdcValue;
    try
    {
        TimeSec time = 0.0;
        for (const auto& value : rawMeasures)
        {
            const double voltage = static_cast<double>(value) * VoltagePerStep;
            m_voltages.emplace_back(std::make_pair(std::ref(time), std::ref(voltage)));
            time += Dt;
        }

        movingFilter(m_voltages);

        // Obliczanie siły
        std::transform(m_voltages.begin(), m_voltages.end(), m_voltages.begin(),
            [](const auto& element)
            {
                return std::make_pair(element.first, calculateForce(element.second));
            });

        // Obliczanie średniej
        auto sum = std::accumulate(m_voltages.begin(), m_voltages.end(), double(0.0), [](auto& accumulated, const auto& el) {
            return accumulated += el.second;
        });
        double mean = static_cast<double>(sum) / static_cast<double>(m_voltages.size());

        // Szukanie momentu uderzenia
        auto [startTick, maxIndex, endTick] = findStartEnd(m_voltages, mean);
        if (endTick > startTick)
        {
            // endTick += AddToEndTickSamples; // Dodaj więcej próbek
            endTick += (endTick - startTick); // Dodaj więcej próbek
            if (endTick >= m_voltages.size())
            {
                endTick = m_voltages.size() - 1;
            }
        }

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

        // Początek uderzenia powinien zaczynać się od 0 w osi Y
        auto firstValue = m_impactVoltages.at(0).second;
        std::transform(m_impactVoltages.begin(), m_impactVoltages.end(), std::back_inserter(m_results),
            [firstValue](const auto& element)
            {
                return std::make_pair(element.first, element.second - firstValue);
            });

        double forceSum = 0.0;
        double forceSumSum = 0.0;
        // Obliczanie przemieszczenia
        std::transform(m_results.begin(), m_results.end(), m_results.begin(),
            [&](const auto& element)
            {
                return calculateDisplacement(element, forceSum, forceSumSum, scale, height);
            });

        m_isOk = true;
    }
    catch (const std::exception& ex)
    {
        qDebug() << "Calculation exception:" << ex.what();
    }
}