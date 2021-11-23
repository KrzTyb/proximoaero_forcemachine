#include "BackendConnector.hpp"

#include <QDebug>

BackendConnector::BackendConnector(QObject *parent) : QObject(parent)
{

}

void BackendConnector::clickStart()
{
    qDebug() << "Start clicked";
    
    emit startClicked();
}

void BackendConnector::setScale(QString scale)
{
    qDebug() << "Set scale: " << scale;

    emit scaleChanged(scale);
}

void BackendConnector::exportData()
{
    emit exportButtonClicked();
}