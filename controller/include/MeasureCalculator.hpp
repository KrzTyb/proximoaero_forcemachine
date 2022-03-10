#pragma once

#include <vector>
#include <utility>

using Voltage = float;
using TimeSec = float;
using VoltageInTime = std::vector<std::pair<TimeSec, Voltage>>;

using Displacement = float;
using Force = float;
using ForceInDisplacement = std::vector<std::pair<Displacement, Force>>;

class MeasureCalculator
{
public:
    // Scale in [kg]
    using Scale = float;
    // Height in [m]
    using Height = float;

    MeasureCalculator(std::vector<int>&& rawMeasures, Scale scale, Height height);

    const VoltageInTime& getVoltages()
    {
        return m_voltages;
    }

    const VoltageInTime& getImpactVoltages()
    {
        return m_impactVoltages;
    }

    const ForceInDisplacement& getResults()
    {
        return m_results;
    }

private:
    VoltageInTime m_voltages;
    VoltageInTime m_impactVoltages;

    ForceInDisplacement m_results;
};