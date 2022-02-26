#include "MeasureController.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QThread>

#include "config.h"
#include <cstdlib>

#include <vector>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "TransportBuilder.hpp"
#include "MeasureCalculator.hpp"
#include "ForceTypes.hpp"

constexpr double FORCE_CONVERT_VALUE = 2000.0 / 9.0;
constexpr double K_TENSO = 1.5;

MeasureController::MeasureController(QObject *parent)
    : QObject(parent)
{

    auto transport = TransportBuilder::createUart();

    m_measureBroker = std::make_shared<MeasureBroker>();
    m_measureBroker->setTransport(std::move(transport));

    connect(m_measureBroker.get(), &MeasureBroker::measureFinished, this, &MeasureController::measureFinished);
}

MeasureController::~MeasureController()
{
}

void MeasureController::measureFinished(const std::string& measures)
{
    // qDebug() << "MeasureFinished: " << QString::fromStdString(measures);

    std::vector<int> rawMeasures;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromStdString(measures));

    MeasureListPtr measureList = QSharedPointer<MeasureList>::create();

    if (jsonDoc.isArray())
    {
        for (const auto& value : jsonDoc.array())
        {
            rawMeasures.emplace_back(value.toInt());
        }

        auto measureCalculator = std::make_shared<MeasureCalculator>(std::move(rawMeasures));

        for (const auto& element : measureCalculator->getVoltages())
        {
            MeasureElement measureElement{element.second, element.first};
            measureList->emplace_back(std::move(measureElement));
        }
        emit measurementsReceived(MeasureStatus::Ok, measureList);
    }
    else
    {
        emit measurementsReceived(MeasureStatus::Error, measureList);
    }
}

#if BUILD_ARM == 1

void MeasureController::startMeasure(int measureTimeMS, int intervalMS)
{
    m_measureBroker->startMeasure();
};

#else

void MeasureController::startMeasure(int measureTimeMS, int intervalMS)
{
    srand(time(NULL));

    QThread::msleep(measureTimeMS);

    double actualTime = 0;

    auto measureListPtr = MeasureListPtr{ new MeasureList{} };

    emit measurementsReceived(MeasureStatus::Ok, measureListPtr);
};

#endif
