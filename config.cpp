#include "config.h"


// Method to load config from JSON
bool Config::LoadFromJson(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open the file for reading:" << file.errorString();
        return false;
    }

    QByteArray fileData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);

    if (jsonDoc.isNull()) {
        qWarning() << "Invalid JSON document";
        return false;
    }

    QJsonObject jsonObject = jsonDoc.object();

    // Populate the class members with values from the JSON object
    if (jsonObject.contains("mpd")) {
        QJsonObject mpdObject = jsonObject["mpd"].toObject();
        mpd_port = mpdObject["port"].toInt();
    }

    return true;
}
