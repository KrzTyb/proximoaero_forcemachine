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

    Q_INVOKABLE void exportData();

signals:

    void startClicked();
    void blockStartClick(bool blocked);
    void blockSetHeightClick(bool blocked);
    void blockExportClick(bool blocked);

    void showDoorPopup(bool visible);
    void showCalibrationPopup(bool visible);
    void openConfigPopup();
    void closeConfigPopup();

    void setWaitPopupState(bool visible);
    void setStartPopupState(bool visible);
    void configEndClicked(int height);

    void scaleChanged(QString scale);

    void measurementsReceived(MeasureList measurements);

    void setCameraVisible(bool visible);
    void setChartVisible(bool visible);

    void exportButtonClicked();
};

