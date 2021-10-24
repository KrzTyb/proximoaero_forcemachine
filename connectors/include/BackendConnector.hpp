#pragma once

#include <QObject>
#include "ForceTypes.hpp"

class BackendConnector : public QObject
{
    Q_OBJECT

public:
    explicit BackendConnector(QObject *parent = nullptr);

    Q_INVOKABLE void clickStart();
    Q_INVOKABLE void setScale(QString scale);

signals:

    void startClicked();
    void blockStartClicked(bool blocked);

    void scaleChanged(QString scale);

    void measurementsReceived(MeasureList measurements);

    void setCameraVisible(bool visible);
    void setChartVisible(bool visible);
};

