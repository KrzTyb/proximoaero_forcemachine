#include "DataSaver.hpp"

#include <QUrl>
#include <QString>
#include <QFile>
#include <QTextStream>

#include <QDebug>
#include <QProcess>

DataSaver::DataSaver(QSharedPointer<BackendConnector> uiConnector, QSharedPointer<USBDeviceHandler> usbHandler, QObject *parent)
    : QObject(parent),
    m_uiConnector {uiConnector},
    m_usbHandler {usbHandler}
{

    connect(usbHandler.get(), &USBDeviceHandler::diskAvailibilityChanged, this,
    [this](auto available)
    {
        if (available && m_measures)
        {
            emit m_uiConnector->blockExportClick(false);
        }
        else
        {
            emit m_uiConnector->blockExportClick(true);
        }
    });

    connect(m_uiConnector.get(), &BackendConnector::exportButtonClicked, this, &DataSaver::onExportClicked);

    m_camera.reset(new QCamera(QMediaDevices::defaultVideoInput()));
    m_captureSession.setCamera(m_camera.data());

    qDebug() << "Camera: " << m_camera->cameraDevice().description();

    if (!m_mediaRecorder)
    {
        m_mediaRecorder.reset(new QMediaRecorder);
        m_captureSession.setRecorder(m_mediaRecorder.data());
    }

    m_mediaRecorder->setOutputLocation(QUrl::fromLocalFile("/tmp/camera.mp4"));
    m_mediaRecorder->setQuality(QMediaRecorder::HighQuality);

    // auto mediaFormat = QMediaFormat{QMediaFormat::FileFormat::AVI};
    auto mediaFormat = QMediaFormat{QMediaFormat::FileFormat::MPEG4};
    mediaFormat.setVideoCodec(QMediaFormat::VideoCodec::H264);

    m_mediaRecorder->setMediaFormat(mediaFormat);
    m_mediaRecorder->setVideoFrameRate(30.0);
    m_mediaRecorder->setEncodingMode(QMediaRecorder::EncodingMode::ConstantQualityEncoding);

    m_camera->start();
}

void DataSaver::onMeasureReceived(MeasureStatus status, MeasureListPtr measurements)
{
    m_mediaRecorder->stop();
    if (status == MeasureStatus::Ok)
    {
        m_measures = measurements;
    }
    else
    {
        qDebug() << "Measure failed!";
        m_measures = nullptr;
    }

    emit captureFinished();

    if (m_usbHandler->isDiskAvailable() && m_measures)
    {
        emit m_uiConnector->blockExportClick(false);
    }
    else
    {
        emit m_uiConnector->blockExportClick(true);
    }
    emit m_uiConnector->blockExportClick(false); // TODO: Remove
}

void DataSaver::startRecording()
{
    m_mediaRecorder->record();
}

void DataSaver::startCapture()
{
    m_measures = nullptr;
    startRecording();
}

void DataSaver::onExportClicked()
{
    if (m_measures == nullptr)
    {
        return;
    }
    if (!m_usbHandler->isDiskAvailable())
    {
        return;
    }

    QString filename {"/tmp/measure.csv"};

    QFile data(filename);
    if(data.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream output(&data);
        // Header
        output << "Siła [N]" << "\t" << "Przemieszczenie [mm]" << "\n";

        // Data
        for (const auto& measureElement : *m_measures)
        {
            output << measureElement.y() << "\t" << measureElement.x() << "\n";
        }

        data.close();
    }

    QProcess syncProcess;

    syncProcess.start("mount " + m_usbHandler->getDiskInfo().path + " /mnt/usb");
    syncProcess.waitForFinished();
    syncProcess.start("cp /tmp/measure.csv /mnt/usb/pomiar.csv");
    syncProcess.waitForFinished();
    syncProcess.start("cp /tmp/camera.mp4 /mnt/usb/wideo.mp4");
    syncProcess.waitForFinished();
    syncProcess.start("umount /mnt/usb");
    syncProcess.waitForFinished();

}