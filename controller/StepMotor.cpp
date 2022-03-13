#include "StepMotor.hpp"

#include <QDebug>

#include <QElapsedTimer>
namespace
{
    constexpr auto STEP_INTERVAL_US = 200;

    constexpr auto STEPS_PER_MILIMETER = 67 * 4;
}

void wait(int32_t microseconds)
{
    static QElapsedTimer timer;
    timer.start();
    while(timer.nsecsElapsed() < (microseconds * 1000));
}

StepMotor::StepMotor(GPIOInputsStates inputStates, QObject *parent)
    : QObject(parent),
    m_motorWorker{inputStates}
{
    qRegisterMetaType<StepDir>();

    connect(this, &StepMotor::lowerLimitStateChanged, &m_motorWorker, &StepMotorWorker::lowerLimitStateChanged);
    connect(this, &StepMotor::upperLimitStateChanged, &m_motorWorker, &StepMotorWorker::upperLimitStateChanged);
    connect(this, &StepMotor::doorStateChanged, &m_motorWorker, &StepMotorWorker::doorStateChanged);
}

StepMotor::~StepMotor()
{

}

void StepMotor::goUp()
{
    stop();
    m_UpThread = QThread::create([this](){m_motorWorker.goUp();});
    if (m_UpThread)
    {
        m_UpThread->start();
    }
}

void StepMotor::goDown()
{
    stop();
    m_DownThread = QThread::create([this](){m_motorWorker.goDown();});
    if (m_DownThread)
    {
        m_DownThread->start();
    }
}

void StepMotor::go(Milimeters milimeters, StepDir dir)
{
    stop();
    m_goThread = QThread::create(
        [this](Milimeters milimeters, StepDir dir)
        {

            m_motorWorker.go(milimeters, dir);

        }, milimeters, dir);
    if (m_goThread)
    {
        connect(m_goThread, &QThread::finished, this, &StepMotor::goFinished);
        m_goThread->start();
    }
}

void StepMotor::stop()
{
    if (m_UpThread && m_UpThread->isRunning())
    {
        qDebug() << "Up suspend";
        m_UpThread->quit();
        m_UpThread = nullptr;
    }
    if (m_DownThread && m_DownThread->isRunning())
    {
        qDebug() << "Down suspend";
        m_DownThread->quit();
        m_DownThread = nullptr;
    }
    if (m_goThread && m_goThread->isRunning())
    {
        qDebug() << "go suspend";
        m_goThread->quit();
        m_goThread = nullptr;
    }
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
    m_enPin.write(true);
    m_dirPin.write(true);
    while (!m_gpioInputStates.upperLimiter)
    {
        if(!m_gpioInputStates.door)
        {
            QThread::msleep(500);
            continue;
        }
        m_pullPin.write(true);
        wait(STEP_INTERVAL_US);
        m_pullPin.write(false);
        wait(STEP_INTERVAL_US);
    }
    m_enPin.write(false);
}

void StepMotorWorker::goDown()
{
    qDebug() << "Go Down";
    m_enPin.write(true);
    m_dirPin.write(false);
    while (!m_gpioInputStates.lowerLimiter)
    {
        if(!m_gpioInputStates.door)
        {
            QThread::msleep(500);
            continue;
        }
        m_pullPin.write(true);
        wait(STEP_INTERVAL_US);
        m_pullPin.write(false);
        wait(STEP_INTERVAL_US);
    }
    m_enPin.write(false);
}

void StepMotorWorker::go(Milimeters milimeters, StepDir dir)
{
    QString dirString { dir == StepDir::Up ? "Up" : "Down"};
    qDebug() << "Go " << dirString << ", " << milimeters << " milimeters";
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
    while (steps)
    {
        if ((dir == StepDir::Up) && m_gpioInputStates.upperLimiter)
        {
            break;
        }
        if(!m_gpioInputStates.door)
        {
            QThread::msleep(500);
            continue;
        }
        m_pullPin.write(true);
        wait(STEP_INTERVAL_US);
        m_pullPin.write(false);
        wait(STEP_INTERVAL_US);
        steps--;
    }
    m_enPin.write(false);
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
