#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QScopedPointer>

#include "ForceTypes.hpp"
#include "BackendConnector.hpp"

#include "USBDeviceHandler.hpp"

class DataSaver : public QObject
{
    Q_OBJECT
    
public:
    DataSaver(QSharedPointer<BackendConnector> uiConnector, QSharedPointer<USBDeviceHandler> usbHandler, QObject *parent = nullptr);

    void clearData()
    {
        m_measures = nullptr;
        emit m_uiConnector->blockExportClick(true);
    };

    MeasureListPtr getMeasurements() { return m_measures; }    

public slots:

    void onMeasureReceived(MeasureStatus status, MeasureListPtr measurements);

    void onExportClicked();

    void scaleChanged(QString scale);

private:

    QSharedPointer<BackendConnector> m_uiConnector;
    QSharedPointer<USBDeviceHandler> m_usbHandler;

    MeasureListPtr  m_measures;

    QString m_scale;

};