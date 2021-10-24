#include "ForceController.hpp"

#include <QTimer>

ForceController::ForceController(QSharedPointer<BackendConnector> uiConnector, QObject *parent)
    : QObject(parent),
      m_uiConnector{std::move(uiConnector)}
{

    if (m_uiConnector)
    {
        connectUI();
    }

    connect(&m_measureController, &MeasureController::measurementsReceived, this,
        [this](auto measurements)
        {
            auto measureList = MeasureList{*measurements};
            measurements = nullptr;
            setChartData(std::move(measureList));
            setChartVisible();
            emit m_uiConnector->blockStartClicked(false);
        });
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

    emit m_measureController.startMeasure();

    QTimer::singleShot(500, this,
    [this]()
    {
        emit m_measureController.stopMeasure();
    });
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
