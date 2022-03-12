#include "VideoController.hpp"

#include <QTimer>
#include <QUrl>

VideoController::VideoController(QObject *parent)
    : QObject(parent)
{
}

void VideoController::initialize()
{
    m_camera.reset(new QCamera(QMediaDevices::defaultVideoInput()));
    m_captureSession.reset(new QMediaCaptureSession());

    m_captureSession->setCamera(m_camera.data());

    qDebug() << "Camera: " << m_camera->cameraDevice().description();

    for (const auto& format : m_camera->cameraDevice().videoFormats())
    {
        if (format.resolution() == QSize(640, 480))
        {
            m_camera->setCameraFormat(format);
            break;
        }
    }

    if (!m_mediaRecorder)
    {
        m_mediaRecorder.reset(new QMediaRecorder);
        m_captureSession->setRecorder(m_mediaRecorder.data());
    }

    m_mediaRecorder->setOutputLocation(QUrl::fromLocalFile("/tmp/camera"));
    m_mediaRecorder->setQuality(QMediaRecorder::NormalQuality);

    auto mediaFormat = QMediaFormat{QMediaFormat::FileFormat::MPEG4};
    mediaFormat.setVideoCodec(QMediaFormat::VideoCodec::H264);

    m_mediaRecorder->setMediaFormat(mediaFormat);
    m_mediaRecorder->setVideoFrameRate(30.0);
    m_mediaRecorder->setVideoResolution(640, 480);
    m_mediaRecorder->setEncodingMode(QMediaRecorder::EncodingMode::ConstantBitRateEncoding);

    if (parent())
    {
        QObject* videoOutput = parent()->findChild<QObject*>("camera_output");
        if (videoOutput)
        {
            m_captureSession->setVideoOutput(videoOutput);
        }
    }

    m_camera->start();
}

void VideoController::startCapture(int captureTimeMs)
{
    QTimer::singleShot(captureTimeMs,
        [this]()
        {
            m_mediaRecorder->stop();
        });
    m_mediaRecorder->record();
}