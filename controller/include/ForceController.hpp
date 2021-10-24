#pragma once

#include <QObject>
#include <QSharedPointer>

#include "BackendConnector.hpp"
#include "MeasureController.hpp"


class ForceController : public QObject
{
    Q_OBJECT
public:
    ForceController(QSharedPointer<BackendConnector> uiConnector, QObject *parent = nullptr);

private slots:

    void onStartClicked();

private:

    void connectUI();

    void executeMeasure();

    void setCameraVisible();
    void setChartData(MeasureList measurements);
    void setChartVisible();
    void hidePreview();

    QSharedPointer<BackendConnector> m_uiConnector;
    MeasureController m_measureController;

    double m_scaleKg {0.0};
};
