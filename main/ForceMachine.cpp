#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>
#include <QSharedPointer>

#include "BackendConnector.hpp"
#include "ForceController.hpp"
#include "ForceTypes.hpp"

#include "USBDeviceHandler.hpp"
#include "MeasureController.hpp"
#include "DataSaver.hpp"
#include "GPIOInputs.hpp"

#include <QDebug>

#include <QTimer>

#include "config.h"
#include <QQuickView>


void debugMenu(QGuiApplication &app, QQmlApplicationEngine &qmlEngine, QSharedPointer<GPIOInputs> &gpioInputs);

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qRegisterMetaType<MeasureList>();
    qRegisterMetaType<MeasureElement>();

    QSharedPointer<BackendConnector> uiConnector = QSharedPointer<BackendConnector>::create();

    QSharedPointer<USBDeviceHandler> usbHandler = QSharedPointer<USBDeviceHandler>::create();
    QSharedPointer<MeasureController> measureController = QSharedPointer<MeasureController>::create();
    QSharedPointer<DataSaver> dataSaver = QSharedPointer<DataSaver>::create(uiConnector, usbHandler);

    QSharedPointer<GPIOInputs> gpioInputs = QSharedPointer<GPIOInputs>::create();
    QPointer<ForceController> forceController = new ForceController(uiConnector, measureController, dataSaver, gpioInputs);

    QThread::sleep(1);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("BackendConnector", uiConnector.get());

    engine.addImportPath(u"qrc:/main"_qs);

#if BUILD_PC == 1
    debugMenu(app, engine, gpioInputs);
#endif

    const QUrl url(u"qrc:/main/Main/main.qml"_qs);
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    QTimer::singleShot(100, forceController.get(), [&forceController](){forceController->initialize();} );

    return app.exec();
}


void debugMenu(QGuiApplication &app, QQmlApplicationEngine &qmlEngine, QSharedPointer<GPIOInputs> &gpioInputs)
{
    qmlEngine.rootContext()->setContextProperty("DebugInput", gpioInputs.get());

    const QUrl debug_url(u"qrc:/main/Main/DebugMenu.qml"_qs);
    QObject::connect(
        &qmlEngine, &QQmlApplicationEngine::objectCreated,
        &app, [debug_url](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && debug_url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    qmlEngine.load(debug_url);
}