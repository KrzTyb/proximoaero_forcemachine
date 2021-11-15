#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QFileSystemWatcher>
#include <QString>
#include <QList>

struct USBDisk
{
    QString path;
    QString name;
};

class USBDeviceHandler : public QObject
{
    Q_OBJECT

public:
    explicit USBDeviceHandler(QObject *parent = nullptr);

    bool isDiskAvailable() { return !usbStorageList.empty(); }

    USBDisk getDiskInfo() { return currentUSBDisk; }

signals:
    void diskAvailibilityChanged(bool available);

public slots:
    void onDirectoryContentChanged(QString);

private:

    /*!
     * \brief Handler do śledzenia zmian podłączonych urządzeń USB
     */
    QSharedPointer<QFileSystemWatcher> usbWatcher;

    /*!
     * \brief Lista urządzeń. Format: </dev/sd*><Tabulator><Nazwa>
     */
    QList<USBDisk> usbStorageList;


    USBDisk currentUSBDisk;

};
