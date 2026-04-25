#include "DeviceManager.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
{
    // Auto-scan configs directory next to the executable
    QString appDir = QCoreApplication::applicationDirPath();
    scanDirectory(appDir + "/configs");
}

void DeviceManager::scanDirectory(const QString &dirPath)
{
    m_deviceNames.clear();
    m_devicePaths.clear();

    QDir dir(dirPath);
    if (!dir.exists()) {
        // Try relative to source
        dir.setPath(QCoreApplication::applicationDirPath() + "/../configs");
    }
    if (!dir.exists()) {
        dir.setPath(QCoreApplication::applicationDirPath() + "/../../configs");
    }

    QStringList filters;
    filters << "*.json";
    const QFileInfoList entries = dir.entryInfoList(filters, QDir::Files, QDir::Name);

    for (const auto &entry : entries) {
        QFile file(entry.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            if (doc.isObject()) {
                const QJsonObject rootObj = doc.object();
                QString name = rootObj["device"].toString();
                if (!name.isEmpty()) {
                    m_deviceNames.append(std::move(name));
                    m_devicePaths.append(entry.absoluteFilePath());
                }
            }
        }
    }

    emit devicesChanged();
}

QString DeviceManager::getConfigPath(int index) const
{
    if (index >= 0 && index < m_devicePaths.size()) {
        return m_devicePaths.at(index);
    }
    return {};
}
