#pragma once

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantList>
#include <QString>
#include <QByteArray>

#include <vector>

struct CodingFieldDef
{
    QString name;
    int bitOffset = 0;
    int bitWidth = 1;
    QString type; // "bool", "enum", "nested", "reserved"
    QVariantList options; // [{value: int, label: string}, ...]
    QString dependsOn;
    std::vector<CodingFieldDef> children;
    QVariantMap dependentOptions; // continent -> [{value,label},...] for dependent enums

    // Delete copy constructor and assignment operator to prevent copying
    CodingFieldDef(const CodingFieldDef&) = delete;
    CodingFieldDef& operator=(const CodingFieldDef&) = delete;
    
    // Allow move constructor and assignment
    CodingFieldDef(CodingFieldDef&&) = default;
    CodingFieldDef& operator=(CodingFieldDef&&) = default;
    
    // Default constructor
    CodingFieldDef() = default;
};

class CodingEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hexString READ hexString WRITE setHexString NOTIFY hexStringChanged)
    Q_PROPERTY(QString binaryString READ binaryString NOTIFY hexStringChanged)
    Q_PROPERTY(int codingBytes READ codingBytes NOTIFY configLoaded)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY configLoaded)

public:
    explicit CodingEngine(QObject *parent = nullptr);

    Q_INVOKABLE bool loadConfig(const QString &filePath);

    QString hexString() const;
    void setHexString(const QString &hex);
    QString binaryString() const;
    int codingBytes() const;
    QString deviceName() const;

    // Bit manipulation
    uint32_t extractBits(int bitOffset, int bitWidth) const;
    void insertBits(int bitOffset, int bitWidth, uint32_t value);

    Q_INVOKABLE void setFieldValue(int bitOffset, int bitWidth, int value);
    Q_INVOKABLE int getFieldValue(int bitOffset, int bitWidth) const;

    const std::vector<CodingFieldDef> &fields() const { return m_fields; }

signals:
    void hexStringChanged();
    void configLoaded();

private:
    CodingFieldDef parseFieldDef(const QJsonObject &obj) const;

    QString m_deviceName;
    int m_codingBytes = 0;
    QByteArray m_data;
    std::vector<CodingFieldDef> m_fields;
};
