#pragma once

#include "GPIOController.hpp"

#include <QObject>


class GPIOOutputs : public QObject
{
    Q_OBJECT
public:
    explicit GPIOOutputs(QObject *parent = nullptr);
    ~GPIOOutputs();
    
    void setRedLedState(bool state) { m_redLed.write(state); }
    void setBlueLedState(bool state) { m_blueLed.write(state); }
    void setGreenLedState(bool state) { m_greenLed.write(state); }
    void setWhiteLedState(bool state) { m_whiteLed.write(state); }

    void setBoltState(bool state) { m_bolt.write(state); }
    void setSupportingElectromagnetState(bool state) { m_supportingElectromagnet.write(state); }

private:
    GPIO m_redLed {16, GPIO_TYPE::OUTPUT, false};
    GPIO m_blueLed {20, GPIO_TYPE::OUTPUT, false};
    GPIO m_greenLed {21, GPIO_TYPE::OUTPUT, false};
    GPIO m_whiteLed {13, GPIO_TYPE::OUTPUT, false};

    GPIO m_bolt {23, GPIO_TYPE::OUTPUT, false};
    GPIO m_supportingElectromagnet {24, GPIO_TYPE::OUTPUT, false};
};
