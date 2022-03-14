#include "DataSaver.hpp"

#include <QUrl>
#include <QString>
#include <QFile>
#include <QTextStream>

#include <QDebug>
#include <QProcess>

#include <QTimer>

#include <QLocale>

constexpr const double InitialScale = 6.0;

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
            QLocale locale{QLocale::Polish};

            QTextStream output(&data);
            // Scale
            output << "Waga: " << m_scale << "kg" << "\n";
            // Header
            output << "Przemieszczenie [m]" << "\t" << "Siła [N]" << "\n";

            // Data
            for (const auto& measureElement : *m_measures)
            {
                output << locale.toString(measureElement.x(), 'f')
                    << "\t" << locale.toString(measureElement.y(), 'f')
                    << "\n";
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
    qDebug() << "Saving to [" << m_usbHandler->getDiskInfo().path << "]";
    if (m_measures == nullptr)
    {
        qDebug() << "Measures nullptr";
        return;
    }
    if (!m_usbHandler->isDiskAvailable())
    {
        qDebug() << "Disk not available";
        return;
    }

    QProcess syncProcess;

    QStringList processArguments;

    auto checkStatus = 
        [&](auto whichProcess)
        {
            if (syncProcess.exitStatus() != QProcess::ExitStatus::NormalExit)
            {
                qDebug() << "Process [" << whichProcess
                    << "] with exit status code: " << syncProcess.exitCode();
            }
        };

    auto checkOutput =
        [&](auto whichProcess)
        {
            qDebug() << "Process [" << whichProcess
                << "] stdout: " << syncProcess.readAllStandardOutput();
            qDebug() << "Process [" << whichProcess
                << "] stderr: " << syncProcess.readAllStandardError();
        };


    processArguments = QStringList{"-p", "/tmp/usb"};
    syncProcess.start("mkdir", processArguments);
    syncProcess.waitForFinished();

    checkOutput("mkdir");

    processArguments = QStringList{m_usbHandler->getDiskInfo().path, "/tmp/usb"};
    syncProcess.start("mount", processArguments);
    syncProcess.waitForFinished();

    checkOutput("Mount");

    processArguments = QStringList{"/tmp/measure.csv", "/tmp/usb/pomiar.csv"};
    syncProcess.start("cp", processArguments);
    syncProcess.waitForFinished();

    checkOutput("CopyMeasure");

    processArguments = QStringList{"/tmp/camera.mp4", "/tmp/usb/wideo.mp4"};
    syncProcess.start("cp", processArguments);
    syncProcess.waitForFinished();

    checkOutput("CopyVideo");

    processArguments = QStringList{"/tmp/usb"};
    syncProcess.start("umount", processArguments);
    syncProcess.waitForFinished();

    checkOutput("Umount");


    qDebug() << "Saving finished";
}

void DataSaver::scaleChanged(QString scale)
{
    QLocale locale{QLocale::Polish};

    bool isOk = false;
    if (auto value = scale.toDouble(&isOk); isOk)
    {
        m_scale = locale.toString(value + InitialScale, 'f', 1);
    }
    else
    {
        m_scale = locale.toString(InitialScale, 'f', 1);
    }
}