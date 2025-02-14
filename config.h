#ifndef CONFIG_H
#define CONFIG_H

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

// Define a class to hold the config data
class Config {
public:
    Config(int mpd_port) : mpd_port(mpd_port) {}

    // Method to load config from JSON
    bool LoadFromJson(const QString &filePath);

private:
    int mpd_port;
};

#endif // CONFIG_H
