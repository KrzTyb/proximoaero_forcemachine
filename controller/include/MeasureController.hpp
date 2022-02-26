#pragma once

#include <QObject>
#include "ForceTypes.hpp"

#include <QJsonArray>

#include <memory>
#include "MeasureBroker.hpp"

constexpr uint32_t MEASURE_TIME_MS = 3000;
constexpr uint32_t MEASURE_INTERVAL_MS = 10;
constexpr double DT = MEASURE_INTERVAL_MS / 1000.0;


class MeasureController : public QObject
{
    Q_OBJECT
public:
    explicit MeasureController(QObject *parent = nullptr);
    ~MeasureController();

    void startMeasure(int measureTimeMS, int intervalMS);
    void setScaleKg(double scaleKg) { m_scaleKg = scaleKg; };
    void setHeightMeters(double meters) { m_heightMeters = meters; };

private slots:
    void measureFinished(const std::string& measures);

signals:
    void measurementsReceived(MeasureStatus status, MeasureListPtr measurements);

private:

    std::shared_ptr<MeasureBroker> m_measureBroker;

    double m_scaleKg = 0.0;
    double m_heightMeters = 0.0;
};
