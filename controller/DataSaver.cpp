#include "DataSaver.hpp"

#include <QUrl>
#include <QString>
#include <QFile>
#include <QTextStream>

#include <QDebug>
#include <QProcess>

#include <QTimer>

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
}

void DataSaver::onMeasureReceived(MeasureStatus status, MeasureListPtr measurements)
{

    if (status == MeasureStatus::Ok)
    {
        m_measures = measurements;
    }
    else
    {
        qDebug() << "Measure failed!";
        m_measures = nullptr;
    }

    if (m_measures)
    {
        qDebug() << "Measure received. Saved to file";
        QString filename {"/tmp/measure.csv"};

        QFile data(filename);
        if(data.open(QFile::WriteOnly | QFile::Truncate))
        {
            QTextStream output(&data);
            // Scale
            output << "Waga: " << m_scale << "kg" << "\n";
            // Header
            output << "Siła [N]" << "\t" << "Przemieszczenie [mm]" << "\n";

            // Data
            for (const auto& measureElement : *m_measures)
            {
                output << measureElement.y() << "\t" << measureElement.x() << "\n";
            }

            data.close();
        }
    }

    if (m_usbHandler->isDiskAvailable() && m_measures)
    {
        emit m_uiConnector->blockExportClick(false);
    }
    else
    {
        emit m_uiConnector->blockExportClick(true);
    }
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

    QProcess syncProcess;

    syncProcess.start("mkdir /mnt/usb");
    syncProcess.waitForFinished();
    syncProcess.start("mount " + m_usbHandler->getDiskInfo().path + " /mnt/usb");
    syncProcess.waitForFinished();
    syncProcess.start("cp /tmp/measure.csv /mnt/usb/pomiar.csv");
    syncProcess.waitForFinished();
    syncProcess.start("cp /tmp/camera.mp4 /mnt/usb/wideo.mp4");
    syncProcess.waitForFinished();
    syncProcess.start("umount /mnt/usb");
    syncProcess.waitForFinished();

}

void DataSaver::scaleChanged(QString scale)
{
    m_scale = scale;
    if (m_scale.isNull())
    {
        m_scale = "0";
    }
}