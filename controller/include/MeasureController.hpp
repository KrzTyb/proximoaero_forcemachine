#pragma once

#include <QObject>
#include <QThread>

#include "ForceTypes.hpp"


class MeasureWorker : public QObject
{
    Q_OBJECT
public slots:
    void startMeasure();
    void stopMeasure();

signals:
    void measureReady(MeasureListPtr measurements);
};


class MeasureController : public QObject
{
    Q_OBJECT
public:
    explicit MeasureController(QObject *parent = nullptr);
    ~MeasureController();

signals:
    void startMeasure();
    void stopMeasure();
    void measurementsReceived(MeasureListPtr measurements);

private:
    QThread workerThread;
};
