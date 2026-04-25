#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickStyle>
#include <QUrl>

#include "CodingEngine.h"
#include "CodingTreeModel.h"
#include "DeviceManager.h"
#include "FileIO.h"

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Basic");
    QGuiApplication app(argc, argv);
    app.setApplicationName("CodingCalculator");
    app.setOrganizationName("CodingCalc");

    QQmlApplicationEngine engine;

    // Create and register backend objects
    DeviceManager deviceManager;
    CodingEngine codingEngine;
    CodingTreeModel treeModel(&codingEngine);
    FileIO fileIO;

    // Create theme object
    QQmlComponent themeComponent(&engine, QUrl("qrc:/qt/qml/CodingCalculator/qml/Theme.qml"));
    QObject *themeObject = themeComponent.create();
    engine.rootContext()->setContextProperty("theme", themeObject);

    engine.rootContext()->setContextProperty("deviceManager", &deviceManager);
    engine.rootContext()->setContextProperty("codingEngine", &codingEngine);
    engine.rootContext()->setContextProperty("treeModel", &treeModel);
    engine.rootContext()->setContextProperty("fileIO", &fileIO);

    const QUrl url(QStringLiteral("qrc:/qt/qml/CodingCalculator/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
