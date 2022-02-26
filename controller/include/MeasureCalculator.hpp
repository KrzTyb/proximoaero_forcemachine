#pragma once

#include <vector>
#include <utility>

using Voltage = float;
using TimeSec = float;
using VoltageInTime = std::vector<std::pair<TimeSec, Voltage>>;


class MeasureCalculator
{
public:
    MeasureCalculator(std::vector<int>&& rawMeasures);

    const VoltageInTime& getVoltages()
    {
        return m_voltages;
    }

private:
    VoltageInTime m_voltages;
};