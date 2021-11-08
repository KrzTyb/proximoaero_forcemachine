#include "ForceController.hpp"

#include <sstream>

#include <GPIOController.hpp>

constexpr uint32_t MEASURE_TIME_MS = 3000;
constexpr uint32_t MEASURE_INTERVAL_MS = 10;


ForceController::ForceController(QSharedPointer<BackendConnector> uiConnector, QObject *parent)
    : QObject(parent),
      m_uiConnector{std::move(uiConnector)},
      m_gpioInputs{},
      m_gpioOutputs{},
      m_stepMotor{ {m_gpioInputs.getLowerLimitState(), m_gpioInputs.getUpperLimitState(), m_gpioInputs.getDoorState()} }
{

    try
    {
        socket.bind("tcp://127.0.0.1:8999");
    }
    catch (zmq::error_t& error)
    {
        qCritical() << "ForceController: Failed bind: " << error.what();
    }

    if (m_uiConnector)
    {
        connectUI();
    }

    connect(&m_gpioInputs, &GPIOInputs::lowerLimitStateChanged, &m_stepMotor, &StepMotor::lowerLimitStateChanged);
    connect(&m_gpioInputs, &GPIOInputs::upperLimitStateChanged, &m_stepMotor, &StepMotor::upperLimitStateChanged);
    connect(&m_gpioInputs, &GPIOInputs::doorStateChanged, &m_stepMotor, &StepMotor::doorStateChanged);

    connect(&m_measureController, &MeasureController::measurementsReceived, this,
        [this](auto status, auto measurements)
        {
            if (status != MeasureStatus::Ok)
            {
                emit m_uiConnector->blockStartClicked(false);
                return;
            }

            logMeasure(measurements);

            auto measureList = MeasureList{*measurements};
            measurements = nullptr;
            setChartData(std::move(measureList));
            setChartVisible();
            emit m_uiConnector->blockStartClicked(false);
        });

    
    emit m_gpioInputs.startLowerLimitListen();
    emit m_gpioInputs.startUpperLimitListen();
    emit m_gpioInputs.startDoorListen();
}

void ForceController::logMeasure(MeasureListPtr measurements)
{
    std::ostringstream oss;

    oss << "Measure [time_ms;force]:\n";
    for (const auto &measure : *measurements)
    {
        oss << measure.x() << ";" << measure.y() << "\n";
    }
    oss << "End measure\n";

    socket.send(
        zmq::buffer(oss.str()),
        zmq::send_flags::none);
}


void ForceController::connectUI()
{
    connect(m_uiConnector.get(), &BackendConnector::startClicked, this, &ForceController::onStartClicked);
    connect(m_uiConnector.get(), &BackendConnector::scaleChanged, this,
    [this](auto scale)
    {
        if (scale.size() == 0)
        {
            m_scaleKg = 0.0;
        }
        else
        {
            m_scaleKg = scale.toDouble();
        }
    });
}

void ForceController::onStartClicked()
{
    emit m_uiConnector->blockStartClicked(true);
    executeMeasure();
}

void ForceController::executeMeasure()
{
    setCameraVisible();

    QMetaObject::Connection upConnection = connect(&m_gpioInputs, &GPIOInputs::upperLimitStateChanged, this,
        [this, &upConnection](auto state)
        {
            if (state)
            {
                disconnect(upConnection);

                QMetaObject::Connection downConnection = connect(&m_gpioInputs, &GPIOInputs::upperLimitStateChanged, this,
                    [this, &downConnection](auto state)
                    {
                        if (state)
                        {
                            emit m_measureController.startMeasure(MEASURE_TIME_MS, MEASURE_INTERVAL_MS);
                            disconnect(downConnection);
                        }
                    });

                emit m_stepMotor.goDown();
            }
        });
    emit m_stepMotor.goUp();
}

void ForceController::setCameraVisible()
{
    emit m_uiConnector->setChartVisible(false);
    emit m_uiConnector->setCameraVisible(true);
}

void ForceController::setChartData(MeasureList measurements)
{
    emit m_uiConnector->measurementsReceived(std::move(measurements));
}

void ForceController::setChartVisible()
{
    emit m_uiConnector->setCameraVisible(false);
    emit m_uiConnector->setChartVisible(true);
}

void ForceController::hidePreview()
{
    emit m_uiConnector->setCameraVisible(false);
    emit m_uiConnector->setChartVisible(false);
}

void ForceController::executeStates()
{

}