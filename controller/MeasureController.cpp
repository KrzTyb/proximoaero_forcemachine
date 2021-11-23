#include "MeasureController.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "config.h"
#include <cstdlib>

constexpr int RECEIVE_TIMEOUT = 10000;

constexpr double FORCE_CONVERT_VALUE = 2000.0 / 9.0;
constexpr double K_TENSO = 1.5;

MeasureWorker::MeasureWorker(QObject *parent)
    : QObject(parent)
{
    try
    {
        socket.connect("ipc:///tmp/ina219_measure");
    }
    catch (zmq::error_t& error)
    {
        qCritical() << "Failed connect: " << error.what();
        return;
    }
}

MeasureWorker::~MeasureWorker()
{
    socket.close();
    context.close();
}

MeasureController::MeasureController(QObject *parent)
    : QObject(parent)
{
    MeasureWorker *measureWorker = new MeasureWorker();
    measureWorker->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, measureWorker, &QObject::deleteLater);
    connect(this, &MeasureController::startMeasure, measureWorker, &MeasureWorker::startMeasure);
    connect(measureWorker, &MeasureWorker::measureReady, this, &MeasureController::measurementsReceived);
    workerThread.start();
}

MeasureController::~MeasureController()
{
    workerThread.quit();
    workerThread.wait();
}

void MeasureWorker::receiveMeasure()
{
    zmq::message_t reply;

    try
    {
        socket.recv(reply);
    }
    catch (zmq::error_t& error)
    {
        qCritical() << "Receive measure failed: " << error.what();
        return;
    }

    auto receivedJSON = QJsonDocument::fromJson(QByteArray::fromStdString(reply.to_string()));
    auto rootJson = receivedJSON.object();
    if (rootJson["status"] == "Ok")
    {
        unpackMeasureAndSend(rootJson["measure"].toArray());
    }
    else
    {
        emit measureReady(MeasureStatus::Error, nullptr);
    }
}

void MeasureWorker::unpackMeasureAndSend(const QJsonArray &jsonMeasurements)
{
    auto measureListPtr = MeasureListPtr{ new MeasureList{} };

    bool initialCalculation = false;
    double initialVoltage = 0.0;
    double initialCurrent = 0.0;

    for (const auto &arrayElement : jsonMeasurements)
    {
        auto jsonObject = arrayElement.toObject();

        double voltage = jsonObject["voltage"].toDouble();
        double current = jsonObject["current"].toDouble();
        if (!initialCalculation)
        {
            measureListPtr->append({voltageToForce(voltage), calculateDisplacement(0.0, voltage, 0.0, current)});
            initialVoltage = voltage;
            initialCurrent = current;
            initialCalculation = true;
        }
        else
        {
            measureListPtr->append({voltageToForce(voltage), calculateDisplacement(initialVoltage, voltage, initialCurrent, current)});
        }
    }

    emit measureReady(MeasureStatus::Ok, measureListPtr);
}

double MeasureWorker::voltageToForce(double voltage)
{
    return FORCE_CONVERT_VALUE * voltage;
}

double MeasureWorker::calculateDisplacement(double initialVoltage, double voltage, double initialCurrent, double current)
{
    try
    {
        return ((voltage - initialVoltage) * initialCurrent)/((current - initialCurrent) * initialVoltage * K_TENSO);
    }
    catch (std::exception &err)
    {
        return 0.0;
    }
}

#if BUILD_ARM == 1

void MeasureWorker::startMeasure(int measureTimeMS, int intervalMS)
{
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &RECEIVE_TIMEOUT, sizeof(RECEIVE_TIMEOUT));

    auto jsonRoot = QJsonObject{};
    jsonRoot["command"] = "StartMeasure";
    jsonRoot["time_ms"] = measureTimeMS;
    jsonRoot["interval_ms"] = intervalMS;
    auto jsonDoc = QJsonDocument{jsonRoot};

    try
    {
        socket.send(
            zmq::buffer(jsonDoc.toJson(QJsonDocument::JsonFormat::Compact).toStdString()),
            zmq::send_flags::none);
    }
    catch (zmq::error_t& error)
    {
        qCritical() << "Send start measure failed: " << error.what();
        return;
    }
    
    receiveMeasure();
};

#else

void MeasureWorker::startMeasure(int measureTimeMS, int intervalMS)
{
    srand(time(NULL));

    QThread::msleep(measureTimeMS);

    double actualTime = 0;

    auto measureListPtr = MeasureListPtr{ new MeasureList{} };

    bool initialCalculation = false;
    double initialVoltage = 0.0;
    double initialCurrent = 0.0;

    while (actualTime < measureTimeMS)
    {
        double voltage = (rand() % 5000) / 1000.0;
        double current = (rand() % 2000) / 1.0;
        if (!initialCalculation)
        {
            measureListPtr->append({voltageToForce(voltage), 0.0});
            initialVoltage = voltage;
            initialCurrent = current;
            initialCalculation = true;
        }
        else
        {
            measureListPtr->append({voltageToForce(voltage), calculateDisplacement(initialVoltage, voltage, initialCurrent, current)});
        }
        actualTime += intervalMS;
    }

    emit measureReady(MeasureStatus::Ok, measureListPtr);

};

#endif
