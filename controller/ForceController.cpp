#include "ForceController.hpp"

#include <sstream>

#include <GPIOController.hpp>

#include <QTimer>

constexpr uint32_t MEASURE_TIME_MS = 3000;
constexpr uint32_t MEASURE_INTERVAL_MS = 10;


ForceController::ForceController(QSharedPointer<BackendConnector> uiConnector,
    QSharedPointer<MeasureController> measureController,
    QSharedPointer<DataSaver> dataSaver,
    QSharedPointer<GPIOInputs> gpioInputs,
    QObject *parent)
    : QObject(parent),
      m_uiConnector{uiConnector},
      m_measureController{measureController},
      m_dataSaver{dataSaver},
      m_gpioInputs{gpioInputs},
      m_gpioOutputs{},
      m_stepMotor{ {m_gpioInputs->getLowerLimitState(), m_gpioInputs->getUpperLimitState(), m_gpioInputs->getDoorState()} }
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

    QObject::connect(m_measureController.get(), &MeasureController::measurementsReceived, m_dataSaver.get(), &DataSaver::onMeasureReceived);

    connect(m_gpioInputs.get(), &GPIOInputs::lowerLimitStateChanged, &m_stepMotor, &StepMotor::lowerLimitStateChanged);
    connect(m_gpioInputs.get(), &GPIOInputs::upperLimitStateChanged, &m_stepMotor, &StepMotor::upperLimitStateChanged);
    connect(m_gpioInputs.get(), &GPIOInputs::doorStateChanged, &m_stepMotor, &StepMotor::doorStateChanged);


    connect(m_gpioInputs.get(), &GPIOInputs::doorStateChanged, this,
    [this](auto closed)
    {
        if (!closed && m_doorPopupPossible)
        {
            emit m_uiConnector->showDoorPopup(true);
        }
        else
        {
            emit m_uiConnector->showDoorPopup(false);
        }
    });
    
    emit m_gpioInputs->startLowerLimitListen();
    emit m_gpioInputs->startUpperLimitListen();
    emit m_gpioInputs->startDoorListen();
    emit m_gpioInputs->startButtonStartListen();
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
    emit m_uiConnector->blockStartClick(true);
    startMeasure();
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

void ForceController::initialize()
{
    m_doorPopupPossible = true;
    if (!m_gpioInputs->getDoorState())
    {
        emit m_uiConnector->showDoorPopup(true);
    }
    else
    {
        emit m_uiConnector->showDoorPopup(false);
    }

    qDebug() << "Initialize started";
    m_gpioOutputs.setSupportingElectromagnetState(false);
    m_gpioOutputs.setBoltState(false);
    m_gpioOutputs.setRedLedState(false);
    m_gpioOutputs.setBlueLedState(false);
    m_gpioOutputs.setGreenLedState(false);
    m_gpioOutputs.setWhiteLedState(false);

    // TODO: Remove
    // emit m_uiConnector->showCalibrationPopup(false);
    // m_ready = true;
    // executeMeasure();
    // return;
    // TODO: Remove

    qDebug() << "Checking door";
    if (!m_gpioInputs->getDoorState())
    {
        qDebug() << "Door opened, waiting for close";
        emit m_uiConnector->showDoorPopup(true);
        m_gpioOutputs.setRedLedState(true);

        QObject *obj = new QObject(this);
        connect(m_gpioInputs.get(), &GPIOInputs::doorStateChanged, obj,
        [this, obj](bool state)
        {
            if (state)
            {
                obj->deleteLater();
                qDebug() << "Door closed";
                emit m_uiConnector->showDoorPopup(false);
                calibration();
            }
        });
    }
    else
    {
        calibration();
    }
}


void ForceController::calibration()
{
    m_doorPopupPossible = true;
    if (!m_gpioInputs->getDoorState())
    {
        emit m_uiConnector->showDoorPopup(true);
    }
    else
    {
        emit m_uiConnector->showDoorPopup(false);
    }

    qDebug() << "Calibration started";
    m_gpioOutputs.setRedLedState(true);
    m_gpioOutputs.setBlueLedState(true);
    m_gpioOutputs.setGreenLedState(true);
    m_gpioOutputs.setWhiteLedState(true);
    emit m_uiConnector->showCalibrationPopup(true);

    if (!m_gpioInputs->getLowerLimitState())
    {
        goDown();
    }
    else
    {
        m_gpioOutputs.setSupportingElectromagnetState(true);
        QTimer::singleShot(500, this, [this](){goUp();});
    }
}

void ForceController::goDown()
{
    m_doorPopupPossible = true;
    if (!m_gpioInputs->getDoorState())
    {
        emit m_uiConnector->showDoorPopup(true);
    }
    else
    {
        emit m_uiConnector->showDoorPopup(false);
    }

    if (m_gpioInputs->getLowerLimitState())
    {
        m_gpioOutputs.setBoltState(true);
        QTimer::singleShot(500, this, [this]()
        {
            m_gpioOutputs.setSupportingElectromagnetState(true);

            QTimer::singleShot(3000, this,
                [this]()
                {
                    m_gpioOutputs.setBoltState(false);
                    QTimer::singleShot(500, this, [this](){goUp();});
                } 
            );
        });
    }
    else
    {
        QObject *obj = new QObject(this);
        connect(m_gpioInputs.get(), &GPIOInputs::lowerLimitStateChanged, obj,
        [this, obj](auto state)
        {
            if (state)
            {
                obj->deleteLater();
                qDebug() << "Low limiter detected";
                m_gpioOutputs.setSupportingElectromagnetState(true);

                QTimer::singleShot(3000, this,
                    [this]()
                    {
                        m_gpioOutputs.setBoltState(false);
                        QTimer::singleShot(500, this, [this](){goUp();});
                    } 
                );
            }
        });
        m_gpioOutputs.setBoltState(true);
        QTimer::singleShot(500, this, [this](){m_stepMotor.goDown();});
    }
}

void ForceController::goUp()
{
    m_doorPopupPossible = true;
    if (!m_gpioInputs->getDoorState())
    {
        emit m_uiConnector->showDoorPopup(true);
    }
    else
    {
        emit m_uiConnector->showDoorPopup(false);
    }

    if (m_gpioInputs->getUpperLimitState())
    {
        qDebug() << "Up limiter detected";
        goHalfMeterFromUp();
    }
    else
    {
        QObject *obj = new QObject(this);
        connect(m_gpioInputs.get(), &GPIOInputs::upperLimitStateChanged, obj,
        [this, obj](auto state)
        {
            if (state)
            {
                obj->deleteLater();
                qDebug() << "Up limiter detected";
                goHalfMeterFromUp();
            }
        });

        m_stepMotor.goUp();
    }

}

void ForceController::goHalfMeterFromUp()
{
    m_doorPopupPossible = true;
    if (!m_gpioInputs->getDoorState())
    {
        emit m_uiConnector->showDoorPopup(true);
    }
    else
    {
        emit m_uiConnector->showDoorPopup(false);
    }

    qDebug() << "Go to 0.5m";
    QObject *obj = new QObject(this);

    connect(&m_stepMotor, &StepMotor::goFinished, obj,
    [this, obj]()
    {
        obj->deleteLater();
        m_gpioOutputs.setRedLedState(false);
        m_gpioOutputs.setBlueLedState(true);
        m_gpioOutputs.setGreenLedState(false);
        m_gpioOutputs.setWhiteLedState(true);
        qDebug() << "Calibration finished";
        emit m_uiConnector->showCalibrationPopup(false);
        m_ready = true;
        m_doorPopupPossible = false;
    });

    m_stepMotor.go(500, StepDir::Down);
}

void ForceController::startMeasure()
{
    m_doorPopupPossible = true;
    if (!m_gpioInputs->getDoorState())
    {
        emit m_uiConnector->showDoorPopup(true);
    }
    else
    {
        emit m_uiConnector->showDoorPopup(false);
    }

    m_dataSaver->clearData();
    if (!m_ready)
    {
        prepareToReady();
        return;
    }
    m_ready = false;

    m_doorPopupPossible = false;
    emit m_uiConnector->showDoorPopup(false);


    emit m_uiConnector->setWaitPopupState(false);
    emit m_uiConnector->openConfigPopup();
    qDebug() << "Preparation started";

    QObject *obj = new QObject(this);

    connect(m_uiConnector.get(), &BackendConnector::configEndClicked, obj,
    [this, obj](auto height)
    {
        obj->deleteLater();
        emit m_uiConnector->closeConfigPopup();
        qDebug() << "Go - height: " << height << " cm";
        emit m_uiConnector->setWaitPopupState(true);
        goToPosition(height * 10); // Convert from cm to mm
    });

}


void ForceController::goToPosition(int heightMilimeters)
{
    QObject *obj = new QObject(this);

    connect(&m_stepMotor, &StepMotor::goFinished, obj,
    [this, obj]()
    {
        obj->deleteLater();
        m_gpioOutputs.setRedLedState(false);
        m_gpioOutputs.setBlueLedState(false);
        m_gpioOutputs.setGreenLedState(true);
        m_gpioOutputs.setWhiteLedState(false);

        m_gpioOutputs.setBoltState(true);

        qDebug() << "Position ready";
        QTimer::singleShot(5000, this,
        [this]()
        {
            QObject *execMeasureObj = new QObject(this);

            connect(m_gpioInputs.get(), &GPIOInputs::startButtonStateChanged, execMeasureObj,
            [this, execMeasureObj](auto state)
            {
                qDebug() << "Mechanical start button clicked";
                if (state)
                {
                    execMeasureObj->deleteLater();
                    emit m_uiConnector->setStartPopupState(false);
                    executeMeasure();
                }
            });

            emit m_uiConnector->setStartPopupState(true);
        });
    });


    int actualPositionMilimeters = 500;

    int diff = heightMilimeters - actualPositionMilimeters;

    if (diff < 0)
    {
        m_stepMotor.go(abs(diff), StepDir::Down);
    }
    else
    {
        m_stepMotor.go(diff, StepDir::Up);
    }
}

void ForceController::executeMeasure()
{
    qDebug() << "Measure started";

    QObject *obj = new QObject(this);

    connect(m_dataSaver.get(), &DataSaver::captureFinished, obj,
    [this, obj]()
    {
        qDebug() << "Measure end";
        obj->deleteLater();

        m_gpioOutputs.setBoltState(false);
        presentation();
    });

    setCameraVisible();
    m_dataSaver->startCapture();
    emit m_measureController->startMeasure(MEASURE_TIME_MS, MEASURE_INTERVAL_MS);

    m_gpioOutputs.setSupportingElectromagnetState(false);
}

void ForceController::presentation()
{
    m_doorPopupPossible = false;
    emit m_uiConnector->showDoorPopup(false);

    auto measurements = m_dataSaver->getMeasurements();

    if (measurements)
    {
        logMeasure(measurements);
        auto measureList = MeasureList{*measurements};
        setChartData(std::move(measureList));
        setChartVisible();
    }
    else
    {
        hidePreview();
    }
    emit m_uiConnector->blockStartClick(false);

}

void ForceController::prepareToReady()
{
    m_gpioOutputs.setRedLedState(true);
    m_gpioOutputs.setBlueLedState(true);
    m_gpioOutputs.setGreenLedState(true);
    m_gpioOutputs.setWhiteLedState(true);
    m_gpioOutputs.setBoltState(true);
    emit m_uiConnector->setWaitPopupState(true);


    QObject *downObj = new QObject(this);
    connect(m_gpioInputs.get(), &GPIOInputs::lowerLimitStateChanged, downObj,
    [this, downObj](auto state)
    {
        if (state)
        {
            downObj->deleteLater();
            qDebug() << "Low limiter detected";
            m_gpioOutputs.setSupportingElectromagnetState(true);
            m_gpioOutputs.setBoltState(false);

            QTimer::singleShot(500, this, [this]()
            {
                m_gpioOutputs.setRedLedState(false);
                m_gpioOutputs.setBlueLedState(true);
                m_gpioOutputs.setGreenLedState(false);
                m_gpioOutputs.setWhiteLedState(true);
                goHalfMeterFromDown();
            });
        }
    });


    QTimer::singleShot(500, this, [this](){m_stepMotor.goDown();});
}

void ForceController::goHalfMeterFromDown()
{
    qDebug() << "Go to 0.5m";
    QObject *obj = new QObject(this);

    connect(&m_stepMotor, &StepMotor::goFinished, obj,
    [this, obj]()
    {
        obj->deleteLater();
        m_ready = true;
        startMeasure();
    });

    m_stepMotor.go(500, StepDir::Up);
}