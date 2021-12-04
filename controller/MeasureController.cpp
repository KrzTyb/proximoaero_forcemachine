#include "MeasureController.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "config.h"
#include <cstdlib>

#include <vector>
#include <cmath>
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
    connect(this, &MeasureController::setScaleKg, measureWorker, &MeasureWorker::setScaleKg);
    connect(this, &MeasureController::setHeightMeters, measureWorker, &MeasureWorker::setHeightMeters);
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
    auto rawMeasureList = std::vector<double>{};

    bool initialCalculation = false;
    double initialVoltage = 0.0;
    double initialCurrent = 0.0;

    for (const auto &arrayElement : jsonMeasurements)
    {
        auto jsonObject = arrayElement.toObject();

        double voltage = jsonObject["voltage"].toDouble();
        // double current = jsonObject["current"].toDouble();
        // if (!initialCalculation)
        // {
        //     // measureListPtr->append({voltageToForce(voltage), current});
        //     rawMeasureListPtr->append({voltage, current});
        //     initialVoltage = voltage;
        //     initialCurrent = current;
        //     initialCalculation = true;
        // }
        // else
        // {
        //     // measureListPtr->append({voltageToForce(voltage), current});
        //     rawMeasureListPtr->append({voltage, current});
        // }
        rawMeasureList.push_back(voltage);
        qDebug() << "Voltage: " << voltage;
    }

    auto measureListPtr = calculateMeasurement(rawMeasureList);

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

std::vector<double> integration(const std::vector<double> &values)
{
    double integral = 0.0;

    auto results = std::vector<double>{};

    for (const auto &value : values)
    {
        integral += value * DT;
        results.push_back(integral);
    }
    return std::move(results);
}

MeasureListPtr MeasureWorker::calculateMeasurement(const std::vector<double> &rawMeasurements)
{
    auto measureListPtr = MeasureListPtr{ new MeasureList{} };

    auto integrationFirst = integration(rawMeasurements);
    for (auto &value : integrationFirst)
    {
        value = std::abs(value);
    }
    const auto integrationResults = integration(integrationFirst);

    int i = 0;
    double time = 0.0;
    for (const auto &value : integrationResults)
    {
        MeasureElement element;
        element.setX(calculateDisplacementNew(value, time));
        element.setY(voltageToForce(rawMeasurements.at(i)));
        measureListPtr->append(element);
        time += DT;
        i++;
    }

    return measureListPtr;
}

double MeasureWorker::calculateDisplacementNew(double value, double timeSec)
{
    constexpr double G = 9.81;

    return std::abs(G * std::sqrt((2.0 * m_heightMeters) / G) * timeSec - (value / (6.0 + m_scaleKg) + ((G*(timeSec*timeSec))/2.0)));
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
