#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QSharedPointer>

#include "BackendConnector.hpp"
#include "ForceController.hpp"
#include "ForceTypes.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qRegisterMetaType<MeasureList>();
    qRegisterMetaType<MeasureElement>();

    QSharedPointer<BackendConnector> uiConnector = QSharedPointer<BackendConnector>::create();


    QPointer<ForceController> forceController = new ForceController(uiConnector);
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("BackendConnector", uiConnector.get());

    const QUrl url(u"qrc:/main/main.qml"_qs);
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl)
        {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
