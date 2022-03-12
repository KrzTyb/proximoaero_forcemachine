#include "USBDeviceHandler.hpp"

#include <QDebug>

#include <QDir>
#include <QStorageInfo>

#include <QProcess>
#include <QPointer>

#include "config.h"


USBDeviceHandler::USBDeviceHandler(QObject *parent) : QObject(parent)
{
}

void USBDeviceHandler::initialize()
{
    usbWatcher = QSharedPointer<QFileSystemWatcher>(new QFileSystemWatcher(this));
    usbWatcher->addPath("/dev/disk/by-label");

    QPointer<QProcess> checkDiskProc = new QProcess();
    #if BUILD_ARM == 1
    checkDiskProc->start("blkid");
    #else
    checkDiskProc->start("blkid");
    #endif
    checkDiskProc->waitForFinished();

    QString output(checkDiskProc->readAllStandardOutput());
    QStringList deviceInfoList = output.split("\n");

    if(usbStorageList.size() > 0)
    {
        usbStorageList.clear();
    }

    for(QString device : deviceInfoList)
    {
        if(device.contains("/dev/sd"))
        {
            QString path;
            QString name;
            path = device.mid(0, device.indexOf(":", 0, Qt::CaseInsensitive));
            int start = device.indexOf("\"", device.indexOf("LABEL=\"", 0, Qt::CaseInsensitive), Qt::CaseInsensitive);
            int stop = device.indexOf("\" ", start+1, Qt::CaseInsensitive);
            name = device.mid(start+1, stop-start-1);
            usbStorageList.append({path, name});
        }
    }

    connect(usbWatcher.get(), &QFileSystemWatcher::directoryChanged, this, &USBDeviceHandler::onDirectoryContentChanged);

    if(usbStorageList.size() > 0)
    {
        currentUSBDisk = usbStorageList.at(0);
        emit diskAvailibilityChanged(true);
    }
    else
    {
        currentUSBDisk = USBDisk{};
        emit diskAvailibilityChanged(false);
    }

    delete checkDiskProc;
}

void USBDeviceHandler::onDirectoryContentChanged(QString path)
{
    QPointer<QProcess> checkDiskProc = new QProcess();
    checkDiskProc->start("blkid");
    checkDiskProc->waitForFinished();

    QString output(checkDiskProc->readAllStandardOutput());
    QStringList deviceInfoList = output.split("\n");


    if(usbStorageList.size() > 0)
    {
        usbStorageList.clear();
    }

    for(QString device : deviceInfoList)
    {
        if(device.contains("/dev/sd"))
        {
            QString path;
            QString name;
            path = device.mid(0, device.indexOf(":", 0, Qt::CaseInsensitive));
            int start = device.indexOf("\"", device.indexOf("LABEL=\"", 0, Qt::CaseInsensitive), Qt::CaseInsensitive);
            int stop = device.indexOf("\" ", start+1, Qt::CaseInsensitive);
            name = device.mid(start+1, stop-start-1);
            usbStorageList.append({path, name});
        }
    }
    if(usbStorageList.size() > 0)
    {
        currentUSBDisk = usbStorageList.at(0);
        emit diskAvailibilityChanged(true);
    }
    else
    {
        currentUSBDisk = USBDisk{};
        emit diskAvailibilityChanged(false);
    }

    delete checkDiskProc;

}