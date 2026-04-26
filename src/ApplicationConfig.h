#pragma once

#include <QObject>
#include <QString>

class ApplicationConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString defaultType MEMBER defaultType CONSTANT)
    Q_PROPERTY(QString nestedType MEMBER nestedType CONSTANT)
    Q_PROPERTY(QString configPath MEMBER configPath CONSTANT)
    Q_PROPERTY(QString defaultTheme MEMBER defaultTheme CONSTANT)
    Q_PROPERTY(bool enableDebugOutput MEMBER enableDebugOutput CONSTANT)

public:
    // Application behavior
    QString defaultType = "reserved";
    QString nestedType = "nested";

    // File paths
    QString configPath = "configs";
    QString defaultTheme = "default";

    // UI settings
    bool enableDebugOutput = true;
    bool autoLoadLastConfig = false;

    // Load from settings file if exists
    void loadFromFile(const QString &configFile);
    void saveToFile(const QString &configFile) const;

    // Get singleton instance
    static ApplicationConfig& instance();

private:
    ApplicationConfig() = default;
};
