#include "DataSaver.hpp"

#include <QDebug>

DataSaver::DataSaver(QSharedPointer<USBDeviceHandler> usbHandler, QObject *parent)
    : QObject(parent),
    m_usbHandler {usbHandler}
{

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

    emit captureFinished();
}

void DataSaver::startRecording()
{

}

void DataSaver::startCapture()
{
    m_measures = nullptr;
    startRecording();
}