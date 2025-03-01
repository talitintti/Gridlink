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
    Config(int mpd_port) : mpd_port_(mpd_port) {}
    Config() : mpd_port_(6600){}

    // Method to load config from JSON
    bool LoadFromJson(const QString &filePath);
    QString GetAsJsonString() const;
    QString GetMusicDir() const;
    int GetMPDPort() const;
private:
    int mpd_port_;
    QString music_dir_;
};

#endif // CONFIG_H
