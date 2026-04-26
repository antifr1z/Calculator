#include "FileIO.h"
#include "CodingEngine.h"

#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

FileIO::FileIO(QObject *parent)
    : QObject(parent)
{
}

bool FileIO::loadHexFromFile(const QString &filePath, CodingEngine *engine)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QTextStream in(&file);
    QString hex = in.readAll().trimmed();

    // Try JSON format: {"coding": "BA34"}
    const QJsonDocument doc = QJsonDocument::fromJson(hex.toUtf8());
    if (doc.isObject()) {
        const QJsonObject obj = doc.object();
        if (obj.contains("coding")) {
            hex = obj["coding"].toString();
        }
    }

    engine->setHexString(hex);
    return true;
}

bool FileIO::saveHexToFile(const QString &filePath, CodingEngine *engine)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QJsonObject obj;
    obj["coding"] = engine->hexString();
    QJsonDocument doc(obj);

    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool FileIO::loadBinaryFromFile(const QString &filePath, CodingEngine *engine)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open binary file for reading:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    engine->setHexString(QString(data.toHex()).toUpper());
    return true;
}

bool FileIO::saveBinaryToFile(const QString &filePath, CodingEngine *engine)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open binary file for writing:" << filePath;
        return false;
    }

    QByteArray data = QByteArray::fromHex(engine->hexString().toLatin1());
    file.write(data);
    return true;
}
