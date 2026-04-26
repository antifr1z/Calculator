#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <memory>

#include "DeviceManager.h"
#include "CodingEngine.h"
#include "CodingTreeModel.h"
#include "FileIO.h"
#include "ApplicationConfig.h"

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);
    ~Application();

    void setupEngine(QQmlApplicationEngine *engine);

private slots:
    void onConfigurationChanged();

private:
    void setupQmlContext();
    void loadConfiguration();
    void connectSignals();
    void createThemeObject();

    // Core components (not owned)
    QQmlApplicationEngine *m_engine = nullptr;
    
    // Backend objects
    std::unique_ptr<DeviceManager> m_deviceManager;
    std::unique_ptr<CodingEngine> m_codingEngine;
    std::unique_ptr<CodingTreeModel> m_treeModel;
    std::unique_ptr<FileIO> m_fileIO;
    
    // Configuration
    ApplicationConfig &m_config;
    
    // UI objects
    QObject *m_themeObject = nullptr;
};
