#pragma once

#include <QObject>
#include <QThread>

#include "ForceTypes.hpp"

#include <QJsonArray>
#include <zmq.hpp>

class MeasureWorker : public QObject
{
    Q_OBJECT
public:
    MeasureWorker(QObject *parent = nullptr);
    ~MeasureWorker();
public slots:
    void startMeasure(int measureTimeMS, int intervalMS);

signals:
    void measureReady(MeasureStatus status, MeasureListPtr measurements);
private:
    void receiveMeasure();
    void unpackMeasureAndSend(const QJsonArray &jsonMeasurements);

    double voltageToForce(double voltage);

    double calculateDisplacement(double initialVoltage, double voltage, double initialCurrent, double current);

    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};
};


class MeasureController : public QObject
{
    Q_OBJECT
public:
    explicit MeasureController(QObject *parent = nullptr);
    ~MeasureController();

signals:
    void startMeasure(int measureTimeMS, int intervalMS);
    void measurementsReceived(MeasureStatus status, MeasureListPtr measurements);

private:
    QThread workerThread;
};
