#include "GPIOInputs.hpp"

#include <QDebug>


static constexpr auto TIMEOUT = ::std::chrono::minutes{1};

GPIOInputs::GPIOInputs(QObject *parent)
    : QObject(parent)
{
    LowerLimiterListener *lowerLimiterListener = new LowerLimiterListener();
    UpperLimiterListener *upperLimiterListener = new UpperLimiterListener();
    DoorListener *doorListener = new DoorListener();
    MechanicalStartButtonListener *startButtonListener = new MechanicalStartButtonListener();

    m_lowerLimitState = !lowerLimiterListener->getState();
    m_upperLimitState = !upperLimiterListener->getState();
    m_doorState = !doorListener->getState();
    m_mechanicalButtonStartState = !startButtonListener->getState();

    qDebug() << "Lower: " << m_lowerLimitState << " Upper: " << m_upperLimitState << " door: " << m_doorState << " startButton: " << m_mechanicalButtonStartState;

    lowerLimiterListener->moveToThread(&lowerLimitThread);
    upperLimiterListener->moveToThread(&upperLimitThread);
    doorListener->moveToThread(&doorThread);
    startButtonListener->moveToThread(&mechanicalStartButtonThread);

    connect(&lowerLimitThread, &QThread::finished, lowerLimiterListener, &QObject::deleteLater);
    connect(&upperLimitThread, &QThread::finished, upperLimiterListener, &QObject::deleteLater);
    connect(&doorThread, &QThread::finished, doorListener, &QObject::deleteLater);
    connect(&mechanicalStartButtonThread, &QThread::finished, startButtonListener, &QObject::deleteLater);

    connect(this, &GPIOInputs::startLowerLimitListen, lowerLimiterListener, &LowerLimiterListener::startListen);
    connect(this, &GPIOInputs::startUpperLimitListen, upperLimiterListener, &UpperLimiterListener::startListen);
    connect(this, &GPIOInputs::startDoorListen, doorListener, &DoorListener::startListen);
    connect(this, &GPIOInputs::startButtonStartListen, startButtonListener, &MechanicalStartButtonListener::startListen);

    connect(lowerLimiterListener, &LowerLimiterListener::stateChanged, this, &GPIOInputs::lowerLimitStateChanged);
    connect(upperLimiterListener, &UpperLimiterListener::stateChanged, this, &GPIOInputs::upperLimitStateChanged);
    connect(doorListener, &DoorListener::stateChanged, this, &GPIOInputs::doorStateChanged);
    connect(startButtonListener, &MechanicalStartButtonListener::stateChanged, this, &GPIOInputs::startButtonStateChanged);

    connect(lowerLimiterListener, &LowerLimiterListener::stateChanged, this,
        [this](auto state)
        {
            m_lowerLimitState = state;
            qDebug() << "lowerLimitState changed: " << m_lowerLimitState;
        });
    connect(upperLimiterListener, &UpperLimiterListener::stateChanged, this,
        [this](auto state)
        {
            m_upperLimitState = state;
            qDebug() << "upperLimitState changed: " << m_upperLimitState;
        });
    connect(doorListener, &DoorListener::stateChanged, this,
        [this](auto state)
        {
            m_doorState = state;
            qDebug() << "doorState changed: " << m_doorState;
        });
    connect(startButtonListener, &MechanicalStartButtonListener::stateChanged, this,
        [this](auto state)
        {
            m_mechanicalButtonStartState = state;
            qDebug() << "startButton changed: " << m_mechanicalButtonStartState;
        });


    lowerLimitThread.start();
    upperLimitThread.start();
    doorThread.start();
    mechanicalStartButtonThread.start();
}

GPIOInputs::~GPIOInputs()
{
    lowerLimitThread.quit();
    upperLimitThread.quit();
    doorThread.quit();
    mechanicalStartButtonThread.quit();

    lowerLimitThread.wait();
    upperLimitThread.wait();
    doorThread.wait();
    mechanicalStartButtonThread.wait();
}

LowerLimiterListener::LowerLimiterListener(QObject *parent)
    : QObject(parent)
{
}

UpperLimiterListener::UpperLimiterListener(QObject *parent)
    : QObject(parent)
{
}

DoorListener::DoorListener(QObject *parent)
    : QObject(parent)
{
}

MechanicalStartButtonListener::MechanicalStartButtonListener(QObject *parent)
    : QObject(parent)
{
}

void LowerLimiterListener::startListen()
{
    while (true)
    {
        auto status = m_gpio.waitToEvent(TIMEOUT);
        if (status)
        {
            auto eventType = m_gpio.getEventType();
            emit stateChanged(eventType == GPIO_EVENT_TYPE::FALLING_EDGE);
        }
    }
}

void UpperLimiterListener::startListen()
{
    while (true)
    {
        auto status = m_gpio.waitToEvent(TIMEOUT);
        if (status)
        {
            auto eventType = m_gpio.getEventType();
            emit stateChanged(eventType == GPIO_EVENT_TYPE::FALLING_EDGE);
        }
    }
}

void DoorListener::startListen()
{
    while (true)
    {
        auto status = m_gpio.waitToEvent(TIMEOUT);
        if (status)
        {
            auto eventType = m_gpio.getEventType();
            emit stateChanged(eventType == GPIO_EVENT_TYPE::FALLING_EDGE);
        }
    }
}

void MechanicalStartButtonListener::startListen()
{
    while (true)
    {
        auto status = m_gpio.waitToEvent(TIMEOUT);
        if (status)
        {
            auto eventType = m_gpio.getEventType();
            emit stateChanged(eventType == GPIO_EVENT_TYPE::FALLING_EDGE);
        }
    }
}