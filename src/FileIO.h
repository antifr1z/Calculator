#pragma once

#include <QObject>
#include <QString>

class CodingEngine;

class FileIO : public QObject
{
    Q_OBJECT

public:
    explicit FileIO(QObject *parent = nullptr);

    Q_INVOKABLE bool loadHexFromFile(const QString &filePath, CodingEngine *engine);
    Q_INVOKABLE bool saveHexToFile(const QString &filePath, CodingEngine *engine);
    Q_INVOKABLE bool loadBinaryFromFile(const QString &filePath, CodingEngine *engine);
    Q_INVOKABLE bool saveBinaryToFile(const QString &filePath, CodingEngine *engine);
};
