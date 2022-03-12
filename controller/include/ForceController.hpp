#pragma once

#include <QObject>
#include <QSharedPointer>

#include "BackendConnector.hpp"
#include "MeasureController.hpp"
#include "DataSaver.hpp"

#include "GPIOInputs.hpp"
#include "GPIOOutputs.hpp"
#include "StepMotor.hpp"

#include <zmq.hpp>


struct ActualLEDStates
{
    bool WHITE = false;
    bool RED = false;
    bool GREEN = false;
    bool BLUE = false;
};

class ForceController : public QObject
{
    Q_OBJECT
public:
    ForceController(QSharedPointer<BackendConnector> uiConnector,
    QSharedPointer<MeasureController> measureController,
    QSharedPointer<DataSaver> dataSaver,
    QSharedPointer<GPIOInputs> gpioInputs,
    QObject *parent = nullptr);

    void initialize();

private slots:

    void onStartClicked();

private:

    void executeStates();

    void connectUI();

    void setCameraVisible();
    void setChartData(MeasureList measurements);
    void setChartVisible();
    void hidePreview();


    void calibration();
    void goDown();
    void goUp();
    void goHalfMeterFromUp();

    void startMeasure();
    void goToPosition(int heightMilimeters);
    void executeMeasure();

    void presentation();

    void prepareToReady();
    void goHalfMeterFromDown();

    QSharedPointer<BackendConnector> m_uiConnector;
    QSharedPointer<MeasureController> m_measureController;
    QSharedPointer<DataSaver>         m_dataSaver;

    double m_scaleKg {0.0};

    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::pub};
    void logMeasure(MeasureListPtr measurements);

    QSharedPointer<GPIOInputs>  m_gpioInputs;
    GPIOOutputs m_gpioOutputs;
    StepMotor   m_stepMotor;

    bool        m_ready {false};

    bool        m_doorPopupPossible {false};

    ActualLEDStates m_ledStates;
};
