#include "CodingEngine.h"
#include "ApplicationConfig.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringView>
#include <QDebug>

CodingEngine::CodingEngine(QObject *parent)
    : QObject(parent)
{
}

bool CodingEngine::loadConfig(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();
    const QString deviceName = root["device"].toString();
    if (deviceName.isEmpty()) {
        qWarning() << "Config file missing required 'device' field";
        return false;
    }
    m_deviceName = deviceName;

    m_codingBytes = root["codingBytes"].toInt(2);
    m_data = QByteArray(m_codingBytes, '\0');
    m_fields.clear();

    const QJsonArray fieldsArr = root["fields"].toArray();
    for (const auto &fieldVal : fieldsArr) {
        m_fields.emplace_back(parseFieldDef(fieldVal.toObject()));
    }

    emit configurationChanged();
    return true;
}

CodingFieldDef CodingEngine::parseFieldDef(const QJsonObject &obj) const
{
    CodingFieldDef def;

    // Validate required fields
    const QString name = obj["name"].toString();
    if (name.isEmpty()) {
        qWarning() << "Field definition missing required 'name' field";
        return def; // Return empty field definition
    }
    
    def.name = name;

    def.bitOffset = obj["bit"].toInt();
    def.bitWidth = obj["width"].toInt(1);
    const QString type = obj["type"].toString(ApplicationConfig::instance().defaultType);
    def.type = type;
    const QString dependsOn = obj["dependsOn"].toString();
    def.dependsOn = dependsOn;

    // Parse simple options array
    if (obj.contains("options") && obj["options"].isArray()) {
        const QJsonArray opts = obj["options"].toArray();
        for (const auto &o : opts) {
            const QJsonObject optObj = o.toObject();
            
            // Validate required fields
            if (!optObj.contains("value") || !optObj.contains("label")) {
                qWarning() << "Option missing required 'value' or 'label' field for field:" << def.name;
                continue; // Skip invalid option
            }
            
            // Convert value safely with validation
            const int value = optObj["value"].toInt(-1); // -1 indicates invalid
            if (value == -1) {
                qWarning() << "Invalid option value for field:" << def.name;
                continue; // Skip invalid option
            }
            
            const QString label = optObj["label"].toString();
            if (label.isEmpty()) {
                qWarning() << "Option has empty label for field:" << def.name;
                continue;
            }
            
            QVariantMap m;
            m["value"] = value;
            m["label"] = label;
            def.options.append(m);
        }
    }

    // Parse dependent options map: { "Europe": [...], "North America": [...] }
    if (obj.contains("options") && obj["options"].isObject()) {
        const QJsonObject depOpts = obj["options"].toObject();
        for (auto it = depOpts.begin(); it != depOpts.end(); ++it) {
            QVariantList list;
            const QJsonArray arr = it.value().toArray();
            for (const auto &o : arr) {
                const QJsonObject optObj = o.toObject();
                
                // Validate dependent option has required fields
                if (!optObj.contains("value") || !optObj.contains("label")) {
                    qWarning() << "Dependent option missing required 'value' or 'label' field for field:" << def.name << "continent:" << it.key();
                    continue; // Skip invalid option
                }
                
                const int value = optObj["value"].toInt(-1); // -1 indicates invalid
                if (value == -1) {
                    qWarning() << "Invalid dependent option value for field:" << def.name << "continent:" << it.key();
                    continue; // Skip invalid option
                }

                const QString label = optObj["label"].toString();
                if (label.isEmpty()) {
                    qWarning() << "Dependent option has empty label for field:" << def.name << "continent:" << it.key();
                    continue;
                }
                
                QVariantMap m;
                m["value"] = value;
                m["label"] = label;
                list.append(m);
            }
            def.dependentOptions[it.key()] = list;
        }
    }

    // Parse children
    if (obj.contains("children")) {
        const QJsonArray childArr = obj["children"].toArray();
        for (const auto &c : childArr) {
            def.children.emplace_back(parseFieldDef(c.toObject()));
        }
    }

    return def;
}

QString CodingEngine::hexString() const
{
    return QString(m_data.toHex()).toUpper();
}

void CodingEngine::setHexString(const QString &hex)
{
    QByteArray newData = QByteArray::fromHex(hex.toLatin1());

    // Pad or truncate to expected size
    if (newData.size() < m_codingBytes) {
        newData.append(QByteArray(m_codingBytes - newData.size(), '\0'));
    } else if (newData.size() > m_codingBytes) {
        newData.truncate(m_codingBytes);
    }

    if (newData != m_data) {
        m_data = newData;
        emit configurationChanged();
    }
}

QString CodingEngine::binaryString() const
{
    if (m_data.isEmpty()) {
        return {};
    }
    // Pre-allocate: 8 binary digits + 1 space per byte, minus trailing space
    const int len = m_data.size() * 9 - 1;
    QString result;
    result.reserve(len);
    for (int i = 0; i < m_data.size(); ++i) {
        if (i > 0) result += QLatin1Char(' ');
        uint8_t byte = static_cast<uint8_t>(m_data[i]);
        for (int b = 7; b >= 0; --b) {
            result += QLatin1Char(((byte >> b) & 1) ? '1' : '0');
        }
    }
    return result;
}

int CodingEngine::codingBytes() const
{
    return m_codingBytes;
}

QString CodingEngine::deviceName() const
{
    return m_deviceName;
}

uint32_t CodingEngine::extractBits(int bitOffset, int bitWidth) const
{
    // Bit 0 = LSB of last byte (little-endian bit numbering)
    if (bitWidth <= 0 || bitWidth > 32 || m_data.isEmpty()) {
        return 0;
    }

    uint32_t result = 0;
    const int dataSize = m_data.size();
    const auto *raw = reinterpret_cast<const uint8_t *>(m_data.constData());
    for (int i = 0; i < bitWidth; ++i) {
        const int globalBit = bitOffset + i;
        const int byteIdx = dataSize - 1 - (globalBit >> 3);
        if (byteIdx >= 0 && byteIdx < dataSize) {
            if (raw[byteIdx] & (1u << (globalBit & 7))) {
                result |= (1u << i);
            }
        }
    }
    return result;
}

void CodingEngine::insertBits(int bitOffset, int bitWidth, uint32_t value)
{
    if (bitWidth <= 0 || bitWidth > 32 || m_data.isEmpty()) {
        return;
    }

    // Check if value actually changes before modifying
    if (extractBits(bitOffset, bitWidth) == value) {
        return;
    }

    const int dataSize = m_data.size();
    auto *raw = reinterpret_cast<uint8_t *>(m_data.data());
    for (int i = 0; i < bitWidth; ++i) {
        const int globalBit = bitOffset + i;
        const int byteIdx = dataSize - 1 - (globalBit >> 3);
        if (byteIdx >= 0 && byteIdx < dataSize) {
            const uint8_t mask = 1u << (globalBit & 7);
            if (value & (1u << i)) {
                raw[byteIdx] |= mask;
            } else {
                raw[byteIdx] &= ~mask;
            }
        }
    }
    emit configurationChanged();
}

void CodingEngine::setFieldValue(int bitOffset, int bitWidth, int value)
{
    insertBits(bitOffset, bitWidth, static_cast<uint32_t>(value));
}

int CodingEngine::getFieldValue(int bitOffset, int bitWidth) const
{
    return static_cast<int>(extractBits(bitOffset, bitWidth));
}
