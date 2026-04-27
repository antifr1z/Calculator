#include "Application.h"
#include <QQmlComponent>
#include <QQmlContext>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QStringList>
#include <QString>

Application::Application(QObject *parent)
    : QObject(parent)
    , m_config(ApplicationConfig::instance())
    , m_deviceManager(std::make_unique<DeviceManager>())
    , m_codingEngine(std::make_unique<CodingEngine>())
    , m_treeModel(std::make_unique<CodingTreeModel>(m_codingEngine.get()))
    , m_fileIO(std::make_unique<FileIO>())
{
    connectSignals();
    loadConfiguration();
}

Application::~Application()
{
    if (m_themeObject) {
        m_themeObject->deleteLater();
        m_themeObject = nullptr;
    }
}

void Application::setupEngine(QQmlApplicationEngine *engine)
{
    m_engine = engine;

    setupQmlContext();
    createThemeObject();
}

void Application::setupQmlContext()
{
    auto *rootContext = m_engine->rootContext();

    rootContext->setContextProperty("appConfig", &m_config);
    rootContext->setContextProperty("deviceManager", m_deviceManager.get());
    rootContext->setContextProperty("codingEngine", m_codingEngine.get());
    rootContext->setContextProperty("treeModel", m_treeModel.get());
    rootContext->setContextProperty("fileIO", m_fileIO.get());
}

void Application::connectSignals()
{
    connect(m_codingEngine.get(), &CodingEngine::configurationChanged,
            this, &Application::onConfigurationChanged);
    
    // Load first device when devices are scanned
    connect(m_deviceManager.get(), &DeviceManager::devicesChanged,
            this, [this]() {
                const QStringList devices = m_deviceManager->deviceNames();
                if (!devices.isEmpty()) {
                    const QString firstConfigPath = m_deviceManager->getConfigPath(0);
                    [[maybe_unused]] bool success = m_codingEngine->loadConfig(firstConfigPath);
                }
            });
}

void Application::loadConfiguration()
{
    const QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);

    const QString configFile = configPath + "/config.json";
    m_config.loadFromFile(configFile);
}

void Application::createThemeObject()
{
    QQmlComponent themeComponent(m_engine, QUrl("qrc:/qml/Theme.qml"));
    m_themeObject = themeComponent.create();
    m_engine->rootContext()->setContextProperty("theme", m_themeObject);
}

void Application::onConfigurationChanged()
{
    if (m_config.enableDebugOutput) {
        const QString deviceName = m_codingEngine->deviceName();
        qDebug() << "Configuration loaded. Device:" << deviceName << "Hex:" << m_codingEngine->hexString();
    }
}
