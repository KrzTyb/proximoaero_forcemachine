#include "MeasureController.hpp"

#include <QRandomGenerator>

MeasureController::MeasureController(QObject *parent)
    : QObject(parent)
{
    MeasureWorker *measureWorker = new MeasureWorker();
    measureWorker->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, measureWorker, &QObject::deleteLater);
    connect(this, &MeasureController::startMeasure, measureWorker, &MeasureWorker::startMeasure);
    connect(this, &MeasureController::stopMeasure, measureWorker, &MeasureWorker::stopMeasure);
    connect(measureWorker, &MeasureWorker::measureReady, this, &MeasureController::measurementsReceived);
    workerThread.start();
}

MeasureController::~MeasureController()
{
    workerThread.quit();
    workerThread.wait();
}


void MeasureWorker::startMeasure()
{

};

void MeasureWorker::stopMeasure()
{
    auto measureListPtr = MeasureListPtr{ new MeasureList{
    {0.5, 0},
    {3.3, 3},
    {60.1, 12},
    {66.6, 31},
    {67.0, 55},
    {67.1, 74},
    {67.1, 78}
    }};

    emit measureReady(measureListPtr);
}
