#include "MeasureController.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "config.h"
#include <cstdlib>

#include <vector>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "TransportBuilder.hpp"
#include "MeasureCalculator.hpp"
#include "ForceTypes.hpp"

#if BUILD_PC == 1
#include "measure.hpp"
#endif

constexpr double FORCE_CONVERT_VALUE = 2000.0 / 9.0;
constexpr double K_TENSO = 1.5;

MeasureController::MeasureController(QObject *parent)
    : QObject(parent)
{

#if BUILD_ARM == 1
    auto transport = TransportBuilder::createUart();

    m_measureBroker = std::make_shared<MeasureBroker>();
    m_measureBroker->setTransport(std::move(transport));

    connect(m_measureBroker.get(), &MeasureBroker::measureFinished, this, &MeasureController::measureFinished);
#else
    m_testTimer.setSingleShot(true);
    connect(&m_testTimer, &QTimer::timeout, this, &MeasureController::pcMeasureFinished);
#endif
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

        auto measureCalculator = std::make_shared<MeasureCalculator>(std::move(rawMeasures), m_scaleKg, m_heightMeters);

        for (const auto& element : measureCalculator->getResults())
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
    m_testTimer.setInterval(measureTimeMS);
    m_testTimer.start();
};

#endif

void MeasureController::pcMeasureFinished()
{
    MeasureListPtr measureList = QSharedPointer<MeasureList>::create();

    std::vector<int> rawMeasures;
    for (const auto& value : ExampleMeasures)
    {
        rawMeasures.emplace_back(value);
    }

    auto measureCalculator = std::make_shared<MeasureCalculator>(std::move(rawMeasures), 6.0f, 0.5f);

    for (const auto& element : measureCalculator->getResults())
    {
        MeasureElement measureElement{element.second, element.first};
        measureList->emplace_back(std::move(measureElement));
    }
    emit measurementsReceived(MeasureStatus::Ok, measureList);
}