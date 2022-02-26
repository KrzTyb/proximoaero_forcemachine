#include "MeasureBroker.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

MeasureBroker::MeasureBroker(QObject *parent)
    : QObject(parent)
{
}

void MeasureBroker::setTransport(std::unique_ptr<ITransport> transport)
{
    m_transport = std::move(transport);

    m_transport->setReceiver(this);
}

std::string MeasureBroker::cleanMessage(const std::string& message)
{
    auto beginOfJson = message.find('{');
    auto endOfJson = message.rfind('}');

    if ((beginOfJson == std::string::npos) || (endOfJson == std::string::npos))
    {
        return "";
    }
    
    std::string clearMsg = message.substr(beginOfJson, endOfJson - beginOfJson + 1);

    return clearMsg;
}

void MeasureBroker::onMessageReceived(const std::string& message)
{
    auto cleanMsg = cleanMessage(message);
    auto jsonMsg = QJsonDocument::fromJson(QByteArray::fromStdString(cleanMsg));

    if (jsonMsg.isNull())
    {
        return;
    }

    switch (m_measureState)
    {
        case MeasureState::Starting:
            {
                if (jsonMsg["type"].toString() == "StartMeasureOk")
                {
                    m_measureState = MeasureState::Measuring;
                }
                else
                {
                    startMeasure();
                }
            }
            break;
        case MeasureState::Measuring:
            {
                if (jsonMsg["type"].toString() == "MeasureReady")
                {
                    m_measureState = MeasureState::ReceivingMeasure;
                    const std::string message = "{\"type\":\"GetNextMeasures\"}";
                    m_transport->send(message);
                }
            }
            break;
        case MeasureState::ReceivingMeasure:
            {
                if (jsonMsg["type"].toString() == "Measures")
                {
                    getMeasurePacket(jsonMsg["measure"]);
                    const std::string message = "{\"type\":\"GetNextMeasures\"}";
                    m_transport->send(message);
                }
                else if (jsonMsg["type"].toString() == "MeasuresReceiveFinished")
                {
                    m_measureState = MeasureState::Idle;
                    emit measureFinished(prepareMeasures());
                }
            }
            break;
        case MeasureState::Idle:
        default:
            break;
    }
}

std::string MeasureBroker::prepareMeasures()
{
    QJsonDocument jsonDoc;
    QJsonArray jsonMeasures;

    for(const auto value : m_rawMeasures)
    {
        jsonMeasures.push_back(value);
    }

    jsonDoc.setArray(jsonMeasures);

    return jsonDoc.toJson(QJsonDocument::JsonFormat::Compact).toStdString();
}

void MeasureBroker::getMeasurePacket(const QJsonValue& measures)
{
    const auto& measureArray = measures.toArray();

    for (const auto& value : measureArray)
    {
        m_rawMeasures.emplace_back(value.toInt());
    }
}

void MeasureBroker::startMeasure()
{
    m_rawMeasures.clear();

    m_measureState = MeasureState::Starting;
    const std::string message = "{\"type\":\"StartMeasure\"}";

    m_transport->send(message);
}

bool MeasureBroker::measureActive()
{
    return m_measureState != MeasureState::Idle;
}
