#pragma once

#include <QObject>
#include <QSharedPointer>

#include "BackendConnector.hpp"
#include "MeasureController.hpp"

#include "GPIOInputs.hpp"
#include "GPIOOutputs.hpp"
#include "StepMotor.hpp"

#include <zmq.hpp>

enum class ForceProgramState
{
    Idle,
    CheckDoor,
    CalibrationDown,
    CalibrationUp,
    Go50cm,
    GetScale,
    GetHeight,
    GoToHeight,
    CheckInputStates,
    Measure,
    ShowResult
};

class ForceController : public QObject
{
    Q_OBJECT
public:
    ForceController(QSharedPointer<BackendConnector> uiConnector, QObject *parent = nullptr);

private slots:

    void onStartClicked();

private:

    void executeStates();

    void connectUI();

    void executeMeasure();

    void setCameraVisible();
    void setChartData(MeasureList measurements);
    void setChartVisible();
    void hidePreview();

    QSharedPointer<BackendConnector> m_uiConnector;
    MeasureController m_measureController;

    double m_scaleKg {0.0};

    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::pub};
    void logMeasure(MeasureListPtr measurements);

    GPIOInputs  m_gpioInputs;
    GPIOOutputs m_gpioOutputs;
    StepMotor   m_stepMotor;
};
