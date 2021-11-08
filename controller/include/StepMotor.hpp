#pragma once

#include <QObject>
#include <QThread>

#include "GPIOController.hpp"

struct GPIOInputsStates
{
    bool lowerLimiter {false};
    bool upperLimiter {false};
    bool door {false};
};

using Milimeters = int64_t;
enum class StepDir
{
    Up,
    Down
};

class StepMotorWorker : public QObject
{
    Q_OBJECT
public:
    StepMotorWorker(GPIOInputsStates gpioInputStates, QObject *parent = nullptr);
    ~StepMotorWorker();
public slots:
    void lowerLimitStateChanged(bool state);
    void upperLimitStateChanged(bool state);
    void doorStateChanged(bool state);

    void goUp();
    void goDown();
    void go(Milimeters milimeters, StepDir dir);
    void stop();

private:
    int64_t milimetersToSteps(Milimeters milimeters);

    GPIO    m_pullPin {17, GPIO_TYPE::OUTPUT};
    GPIO    m_dirPin {27, GPIO_TYPE::OUTPUT};
    GPIO    m_enPin {22, GPIO_TYPE::OUTPUT, true};

    GPIOInputsStates    m_gpioInputStates;

    bool running {false};
};

class StepMotor : public QObject
{
    Q_OBJECT
public:
    explicit StepMotor(GPIOInputsStates inputStates, QObject *parent = nullptr);
    ~StepMotor();

signals:
    void lowerLimitStateChanged(bool state);
    void upperLimitStateChanged(bool state);
    void doorStateChanged(bool state);

    void goUp();
    void goDown();
    void go(Milimeters milimeters, StepDir dir);
    void stop();

private:
    QThread stepMotorThread;
};