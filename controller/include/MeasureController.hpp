#pragma once

#include <QObject>
#include <QThread>

#include "ForceTypes.hpp"

#include <QJsonArray>
#include <zmq.hpp>

constexpr uint32_t MEASURE_TIME_MS = 3000;
constexpr uint32_t MEASURE_INTERVAL_MS = 10;
constexpr double DT = MEASURE_INTERVAL_MS / 1000.0;

class MeasureWorker : public QObject
{
    Q_OBJECT
public:
    MeasureWorker(QObject *parent = nullptr);
    ~MeasureWorker();

public slots:
    void startMeasure(int measureTimeMS, int intervalMS);
    void setScaleKg(double scaleKg) { m_scaleKg = scaleKg; };
    void setHeightMeters(double meters) { m_heightMeters = meters; };

signals:
    void measureReady(MeasureStatus status, MeasureListPtr measurements);
private:
    void receiveMeasure();
    void unpackMeasureAndSend(const QJsonArray &jsonMeasurements);

    double voltageToForce(double voltage);

    double calculateDisplacement(double initialVoltage, double voltage, double initialCurrent, double current);

    MeasureListPtr calculateMeasurement(const std::vector<double> &rawMeasurements);
    double calculateDisplacementNew(double value, double timeSec);

    zmq::context_t context{1};
    zmq::socket_t socket{context, zmq::socket_type::req};

    double m_scaleKg = 0.0;
    double m_heightMeters = 0.0;
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

    void setScaleKg(double scaleKg);
    void setHeightMeters(double meters);

private:
    QThread workerThread;
};
