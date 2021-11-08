#include "StepMotor.hpp"

namespace
{
    constexpr auto STEP_INTERVAL_US = 500;

    constexpr auto STEPS_PER_MILIMETER = 200;
}

StepMotor::StepMotor(GPIOInputsStates inputStates, QObject *parent)
    : QObject(parent)
{
    StepMotorWorker *stepMotorWorker = new StepMotorWorker(inputStates);
    stepMotorWorker->moveToThread(&stepMotorThread);

    connect(&stepMotorThread, &QThread::finished, stepMotorWorker, &QObject::deleteLater);

    connect(this, &StepMotor::lowerLimitStateChanged, stepMotorWorker, &StepMotorWorker::lowerLimitStateChanged);
    connect(this, &StepMotor::upperLimitStateChanged, stepMotorWorker, &StepMotorWorker::upperLimitStateChanged);
    connect(this, &StepMotor::doorStateChanged, stepMotorWorker, &StepMotorWorker::doorStateChanged);

    connect(this, &StepMotor::goUp, stepMotorWorker, &StepMotorWorker::goUp);
    connect(this, &StepMotor::goDown, stepMotorWorker, &StepMotorWorker::goDown);
    connect(this, &StepMotor::go, stepMotorWorker, &StepMotorWorker::go);
    connect(this, &StepMotor::stop, stepMotorWorker, &StepMotorWorker::stop);

    stepMotorThread.start();
}

StepMotor::~StepMotor()
{
    stepMotorThread.quit();
    stepMotorThread.wait();
}


StepMotorWorker::StepMotorWorker(GPIOInputsStates gpioInputStates, QObject *parent)
    : QObject(parent),
    m_gpioInputStates{ gpioInputStates }
{

}

StepMotorWorker::~StepMotorWorker()
{

}

void StepMotorWorker::lowerLimitStateChanged(bool state)
{
    m_gpioInputStates.lowerLimiter = state;
}

void StepMotorWorker::upperLimitStateChanged(bool state)
{
    m_gpioInputStates.upperLimiter = state;
}

void StepMotorWorker::doorStateChanged(bool state)
{
    m_gpioInputStates.door = state;
}

void StepMotorWorker::goUp()
{
    running = true;
    m_enPin.write(false);
    m_dirPin.write(false);
    while (!m_gpioInputStates.upperLimiter && running)
    {
        m_pullPin.write(true);
        QThread::usleep(STEP_INTERVAL_US);
        m_pullPin.write(false);
        QThread::usleep(STEP_INTERVAL_US);
    }
    m_enPin.write(true);
    running = false;
}

void StepMotorWorker::goDown()
{
    running = true;
    m_enPin.write(false);
    m_dirPin.write(true);
    while (!m_gpioInputStates.lowerLimiter && running)
    {
        m_pullPin.write(true);
        QThread::usleep(STEP_INTERVAL_US);
        m_pullPin.write(false);
        QThread::usleep(STEP_INTERVAL_US);
    }
    m_enPin.write(true);
    running = false;
}

void StepMotorWorker::go(Milimeters milimeters, StepDir dir)
{
    running = true;
    auto steps = milimetersToSteps(milimeters);
    m_enPin.write(false);
    if (dir == StepDir::Up)
    {
        m_dirPin.write(false);
    }
    else
    {
        m_dirPin.write(true);
    }
    while (steps && running)
    {
        if ((dir == StepDir::Down) && m_gpioInputStates.lowerLimiter)
        {
            break;
        }
        else if ((dir == StepDir::Up) && m_gpioInputStates.upperLimiter)
        {
            break;
        }
        m_pullPin.write(true);
        QThread::usleep(STEP_INTERVAL_US);
        m_pullPin.write(false);
        QThread::usleep(STEP_INTERVAL_US);
        steps--;
    }
    m_enPin.write(true);
    running = false;
}

void StepMotorWorker::stop()
{
    running = false;
}

int64_t StepMotorWorker::milimetersToSteps(Milimeters milimeters)
{
    return milimeters * STEPS_PER_MILIMETER;
}