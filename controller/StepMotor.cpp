#include "StepMotor.hpp"

#include <QDebug>

#include <QtConcurrent>

#include <QFuture>
namespace
{
    constexpr auto STEP_INTERVAL_US = 500;

    constexpr auto STEPS_PER_MILIMETER = 67;
}

StepMotor::StepMotor(GPIOInputsStates inputStates, QObject *parent)
    : QObject(parent),
    m_motorWorker{inputStates}
{
    qRegisterMetaType<StepDir>();

    connect(&m_goWatcher, &QFutureWatcher<void>::finished, this, &StepMotor::goFinished);
}

StepMotor::~StepMotor()
{

}

void StepMotor::goUp()
{
    QtConcurrent::run([this](){m_motorWorker.goUp();});
}

void StepMotor::goDown()
{
    QtConcurrent::run([this](){m_motorWorker.goDown();});
}

void StepMotor::go(Milimeters milimeters, StepDir dir)
{
    QFuture<void> future = QtConcurrent::run(
        [this](Milimeters milimeters, StepDir dir)
        {

            m_motorWorker.go(milimeters, dir);

        }, milimeters, dir);
    m_goWatcher.setFuture(future);
}

void StepMotor::stop()
{
    QtConcurrent::run([this](){m_motorWorker.stop();});
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
    qDebug() << "Go Up";
    running = true;
    m_enPin.write(true);
    m_dirPin.write(true);
    while (!m_gpioInputStates.upperLimiter && running)
    {
        m_pullPin.write(true);
        QThread::usleep(STEP_INTERVAL_US);
        m_pullPin.write(false);
        QThread::usleep(STEP_INTERVAL_US);
    }
    m_enPin.write(false);
    running = false;
}

void StepMotorWorker::goDown()
{
    qDebug() << "Go Down";
    running = true;
    m_enPin.write(true);
    m_dirPin.write(false);
    while (!m_gpioInputStates.lowerLimiter && running)
    {
        m_pullPin.write(true);
        QThread::usleep(STEP_INTERVAL_US);
        m_pullPin.write(false);
        QThread::usleep(STEP_INTERVAL_US);
    }
    m_enPin.write(false);
    running = false;
}

void StepMotorWorker::go(Milimeters milimeters, StepDir dir)
{
    QString dirString { dir == StepDir::Up ? "Up" : "Down"};
    qDebug() << "Go " << dirString << ", " << milimeters << " milimeters";
    running = true;
    auto steps = milimetersToSteps(milimeters);
    qDebug() << "Steps: " << steps;
    m_enPin.write(true);
    if (dir == StepDir::Up)
    {
        m_dirPin.write(true);
    }
    else
    {
        m_dirPin.write(false);
    }
    while (steps && running)
    {
        // if ((dir == StepDir::Down) && m_gpioInputStates.lowerLimiter)
        // {
        //     break;
        // }
        // else if ((dir == StepDir::Up) && m_gpioInputStates.upperLimiter)
        // {
        //     break;
        // }
        m_pullPin.write(true);
        QThread::usleep(STEP_INTERVAL_US);
        m_pullPin.write(false);
        QThread::usleep(STEP_INTERVAL_US);
        steps--;
    }
    m_enPin.write(false);
    running = false;
    qDebug() << "Go on position finished";
    emit goFinished();
}

void StepMotorWorker::stop()
{
    running = false;
}

int64_t StepMotorWorker::milimetersToSteps(Milimeters milimeters)
{
    return milimeters * STEPS_PER_MILIMETER;
}