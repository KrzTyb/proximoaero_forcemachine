#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QScopedPointer>

#include "ForceTypes.hpp"
#include "BackendConnector.hpp"

#include "USBDeviceHandler.hpp"

#include <QCamera>
#include <QMediaRecorder>
#include <QMediaMetaData>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaCaptureSession>

class DataSaver : public QObject
{
    Q_OBJECT
    
public:
    DataSaver(QSharedPointer<BackendConnector> uiConnector, QSharedPointer<USBDeviceHandler> usbHandler, QObject *parent = nullptr);

    void startCapture();
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

signals:
    void captureFinished();

private:

    void startRecording();

    QSharedPointer<BackendConnector> m_uiConnector;
    QSharedPointer<USBDeviceHandler> m_usbHandler;

    MeasureListPtr  m_measures;


    QMediaDevices m_devices;
    QMediaCaptureSession m_captureSession;
    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QMediaRecorder> m_mediaRecorder;

    QString m_scale;

};