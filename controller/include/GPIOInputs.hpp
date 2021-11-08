#pragma once

#include "GPIOController.hpp"

#include <QObject>
#include <QThread>

class GPIOInputs;

class LowerLimiterListener : public QObject
{
    Q_OBJECT
    friend class GPIOInputs;
public:
    LowerLimiterListener(QObject *parent = nullptr);
    ~LowerLimiterListener() = default;
public slots:
    void startListen();

signals:
    void stateChanged(bool state);

private:
    bool getState() { return m_gpio.read(); }
    GPIO m_gpio {5, GPIO_TYPE::BOTH_EDGES};
};

class UpperLimiterListener : public QObject
{
    Q_OBJECT
    friend class GPIOInputs;
public:
    UpperLimiterListener(QObject *parent = nullptr);
    ~UpperLimiterListener() = default;
public slots:
    void startListen();

signals:
    void stateChanged(bool state);

private:
    bool getState() { return m_gpio.read(); }

    GPIO m_gpio {12, GPIO_TYPE::BOTH_EDGES};
};

class DoorListener : public QObject
{
    Q_OBJECT
    friend class GPIOInputs;
public:
    DoorListener(QObject *parent = nullptr);
    ~DoorListener() = default;
public slots:
    void startListen();

signals:
    void stateChanged(bool state);

private:
    bool getState() { return m_gpio.read(); }

    GPIO m_gpio {19, GPIO_TYPE::BOTH_EDGES};
};

class GPIOInputs : public QObject
{
    Q_OBJECT
public:
    explicit GPIOInputs(QObject *parent = nullptr);
    ~GPIOInputs();

    bool getLowerLimitState() { return m_lowerLimitState; }
    bool getUpperLimitState() { return m_upperLimitState; }
    bool getDoorState() { return m_doorState; }

signals:
    void startLowerLimitListen();
    void startUpperLimitListen();
    void startDoorListen();

    void lowerLimitStateChanged(bool state);
    void upperLimitStateChanged(bool state);
    void doorStateChanged(bool state);

private:
    QThread lowerLimitThread;
    QThread upperLimitThread;
    QThread doorThread;

    bool m_lowerLimitState {false};
    bool m_upperLimitState {false};
    bool m_doorState {false};
};
