#include "ApplicationConfig.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

ApplicationConfig& ApplicationConfig::instance()
{
    static ApplicationConfig config;
    return config;
}

void ApplicationConfig::loadFromFile(const QString &configFile)
{
    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return;
    }

    const QJsonObject obj = doc.object();

    defaultType = obj["defaultType"].toString(defaultType);
    nestedType = obj["nestedType"].toString(nestedType);
    configPath = obj["configPath"].toString(configPath);
    defaultTheme = obj["defaultTheme"].toString(defaultTheme);
    enableDebugOutput = obj["enableDebugOutput"].toBool(enableDebugOutput);
    autoLoadLastConfig = obj["autoLoadLastConfig"].toBool(autoLoadLastConfig);
}

void ApplicationConfig::saveToFile(const QString &configFile) const
{
    QJsonObject obj;
    obj["defaultType"] = defaultType;
    obj["nestedType"] = nestedType;
    obj["configPath"] = configPath;
    obj["defaultTheme"] = defaultTheme;
    obj["enableDebugOutput"] = enableDebugOutput;
    obj["autoLoadLastConfig"] = autoLoadLastConfig;
    
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    }
}
