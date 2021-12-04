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
    bool getState() { return m_actualState; }
    GPIO m_gpio {5, GPIO_TYPE::BOTH_EDGES};
    bool m_actualState;
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
    bool getState() { return m_actualState; }

    GPIO m_gpio {12, GPIO_TYPE::BOTH_EDGES};
    bool m_actualState;
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
    bool getState() { return m_actualState; }

    GPIO m_gpio {19, GPIO_TYPE::BOTH_EDGES};
    bool m_actualState;
};

class MechanicalStartButtonListener : public QObject
{
    Q_OBJECT
    friend class GPIOInputs;
public:
    MechanicalStartButtonListener(QObject *parent = nullptr);
    ~MechanicalStartButtonListener() = default;
public slots:
    void startListen();

signals:
    void stateChanged(bool state);

private:
    bool getState() { return m_actualState; }

    GPIO m_gpio {26, GPIO_TYPE::BOTH_EDGES};
    bool m_actualState;
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
    bool getButtonStartState() { return m_mechanicalButtonStartState; }

    Q_INVOKABLE void debug_lowerLimitState(bool state) { m_lowerLimitState = state; emit lowerLimitStateChanged(m_lowerLimitState); };
    Q_INVOKABLE void debug_upperLimitState(bool state) { m_upperLimitState = state; emit upperLimitStateChanged(m_upperLimitState); };
    Q_INVOKABLE void debug_doorState(bool state) { m_doorState = state; emit doorStateChanged(m_doorState); };
    Q_INVOKABLE void debug_startButtonState(bool state) { m_mechanicalButtonStartState = state; emit startButtonStateChanged(m_mechanicalButtonStartState); };

signals:
    void startLowerLimitListen();
    void startUpperLimitListen();
    void startDoorListen();
    void startButtonStartListen();

    void lowerLimitStateChanged(bool state);
    void upperLimitStateChanged(bool state);
    void doorStateChanged(bool state);
    void startButtonStateChanged(bool state);

private:
    QThread lowerLimitThread;
    QThread upperLimitThread;
    QThread doorThread;
    QThread mechanicalStartButtonThread;

    bool m_lowerLimitState {false};
    bool m_upperLimitState {false};
    bool m_doorState {false};
    bool m_mechanicalButtonStartState {false};
};
