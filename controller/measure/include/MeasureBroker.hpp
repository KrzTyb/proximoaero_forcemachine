#pragma once

#include <QObject>

#include "TransportDelegate.hpp"

#include <ITransport.hpp>

#include <string>
#include <vector>

enum class MeasureState
{
    Idle,
    Starting,
    Measuring,
    ReceivingMeasure
};

class MeasureBroker : public QObject, public TransportDelegate
{
    Q_OBJECT
public:
    explicit MeasureBroker(QObject *parent = nullptr);

    void setTransport(std::unique_ptr<ITransport> transport);

    virtual ~MeasureBroker() = default;

    void startMeasure();
    bool measureActive();

signals:
    void measureFinished(const std::string& measures);

private:
    std::string cleanMessage(const std::string& message);

    void onMessageReceived(const std::string& message) override;

    void getMeasurePacket(const QJsonValue& measures);

    std::string prepareMeasures();

    std::unique_ptr<ITransport> m_transport;

    MeasureState m_measureState;

    std::vector<int> m_rawMeasures;

};
