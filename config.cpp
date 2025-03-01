#include "config.h"


// Method to load config from JSON
bool Config::LoadFromJson(const QString &file_path) {
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open the file for reading:" << file.errorString();
        return false;
    }

    QByteArray file_data = file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(file_data);

    if (json_doc.isNull()) {
        qWarning() << "Invalid JSON document";
        return false;
    }

    QJsonObject json_object = json_doc.object();

    if (json_object.contains("mpd")) {
        mpd_port_ = json_object["port"].toInt();
    }

    if (json_object.contains("music_dir")) {
        music_dir_ = json_object["music_dir"].toString();
    }

    return true;
}

QString Config::GetAsJsonString() const {
    QJsonObject config_obj;

    config_obj["mpd_port"] = mpd_port_;
    config_obj["music_dir"] = music_dir_;

    QJsonDocument jsonDoc(config_obj);

    return jsonDoc.toJson(QJsonDocument::Indented);
}

QString Config::GetMusicDir() const {
    return music_dir_;
}

int Config::GetMPDPort() const {
    return mpd_port_;
}
