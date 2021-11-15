#pragma once

#include <QObject>
#include <QSharedPointer>

#include "ForceTypes.hpp"

#include "USBDeviceHandler.hpp"

class DataSaver : public QObject
{
    Q_OBJECT
    
public:
    DataSaver(QSharedPointer<USBDeviceHandler> usbHandler, QObject *parent = nullptr);

    void startCapture();

    MeasureListPtr getMeasurements() { return m_measures; }    

public slots:

    void onMeasureReceived(MeasureStatus status, MeasureListPtr measurements);

signals:
    void captureFinished();

private:

    void startRecording();

    QSharedPointer<USBDeviceHandler> m_usbHandler;

    MeasureListPtr  m_measures;

};