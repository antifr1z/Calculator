#pragma once

#include <QObject>
#include <QStringList>

class DeviceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList deviceNames READ deviceNames NOTIFY devicesChanged)
    Q_PROPERTY(QStringList devicePaths READ devicePaths NOTIFY devicesChanged)

public:
    explicit DeviceManager(QObject *parent = nullptr);

    Q_INVOKABLE void scanDirectory(const QString &dirPath);
    [[nodiscard]] Q_INVOKABLE QString getConfigPath(int index) const;

    [[nodiscard]] const QStringList &deviceNames() const { return m_deviceNames; }
    [[nodiscard]] const QStringList &devicePaths() const { return m_devicePaths; }

signals:
    void devicesChanged();

private:
    QStringList m_deviceNames;
    QStringList m_devicePaths;
};
