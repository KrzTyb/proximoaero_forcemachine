#include "MeasureController.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

constexpr int RECEIVE_TIMEOUT = 10000;

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

    for (const auto &arrayElement : jsonMeasurements)
    {
        auto jsonObject = arrayElement.toObject();
        measureListPtr->append({voltageToForce(jsonObject["voltage"].toDouble()), jsonObject["time_ms"].toDouble()});
    }

    emit measureReady(MeasureStatus::Ok, measureListPtr);
}

constexpr double FORCE_CONVERT_VALUE = 2000.0 / 9.0;

double MeasureWorker::voltageToForce(double voltage)
{
    return FORCE_CONVERT_VALUE * voltage;
}
