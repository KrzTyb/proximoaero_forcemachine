#pragma once

#include <QObject>
#include <QThread>

#include "GPIOController.hpp"
#include <QFutureWatcher>

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

Q_DECLARE_METATYPE(StepDir);

class StepMotorWorker : public QObject
{
    Q_OBJECT
public:
    StepMotorWorker(GPIOInputsStates gpioInputStates, QObject *parent = nullptr);
    ~StepMotorWorker();

    void goUp();
    void goDown();
    void go(Milimeters milimeters, StepDir dir);
    void stop();

public slots:
    void lowerLimitStateChanged(bool state);
    void upperLimitStateChanged(bool state);
    void doorStateChanged(bool state);

signals:
    void goFinished();

private:
    int64_t milimetersToSteps(Milimeters milimeters);

    GPIO    m_pullPin {17, GPIO_TYPE::OUTPUT, false, OUTPUT_TYPE::OPEN_DRAIN};
    GPIO    m_dirPin {27, GPIO_TYPE::OUTPUT, false, OUTPUT_TYPE::OPEN_DRAIN};
    GPIO    m_enPin {22, GPIO_TYPE::OUTPUT, true, OUTPUT_TYPE::OPEN_DRAIN};

    GPIOInputsStates    m_gpioInputStates;

    bool running {false};
};

class StepMotor : public QObject
{
    Q_OBJECT
public:
    explicit StepMotor(GPIOInputsStates inputStates, QObject *parent = nullptr);
    ~StepMotor();

    void goUp();
    void goDown();
    void go(Milimeters milimeters, StepDir dir);
    void stop();

signals:
    void lowerLimitStateChanged(bool state);
    void upperLimitStateChanged(bool state);
    void doorStateChanged(bool state);
    void goFinished();

private:
    QThread stepMotorThread;
    StepMotorWorker m_motorWorker;
    QFutureWatcher<void> m_goWatcher;
};