#pragma once

#include <QObject>

#include <QCamera>
#include <QMediaRecorder>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaCaptureSession>

class VideoController : public QObject
{
public:

    explicit VideoController(QObject *parent = nullptr);

    void initialize();

    void startCapture(int captureTimeMs);

private:

    QScopedPointer<QMediaDevices> m_devices;
    QScopedPointer<QMediaCaptureSession> m_captureSession;
    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QMediaRecorder> m_mediaRecorder;

};