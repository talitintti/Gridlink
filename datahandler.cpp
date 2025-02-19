#include "datahandler.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDebug>

DataHandler::DataHandler() {}
DataHandler::~DataHandler() {
}

bool DataHandler::Initialize() {
    config_ = ReadConfigFile();

    if (!mpd_communicator_.Initialize()) {
        return false;
    }

    return true;
}

QList<QString> DataHandler::GetArtistNames() {
    return mpd_communicator_.GetArtists(QString("AlbumArtist")); // maybe config file decides artist_type
}

QList<QString> DataHandler::GetAlbumsForArtist(const QString &artist_name) {
    auto list = mpd_communicator_.GetAlbumsOfAnArtist(artist_name);
    return list;
}




Config DataHandler::ReadConfigFile() {

    // Figuring out where to config file should go
    QString configFolderPath = QStandardPaths::writableLocation(
        QStandardPaths::ConfigLocation
        );

    // If XDG_DEFAULT_HOME is not set
    if (configFolderPath.isEmpty()) {
        qWarning() << "Defeault config path is not set\n";
        //TODO: signal slot error here
    }

    QString configFilePath = configFolderPath + "/Gridlink/config.yaml";
    QFile configFile(configFilePath);

    if (!configFile.exists()) {
        Config default_config = Config();
        WriteConfigFile(configFile, default_config);
        return default_config;
    }

    // The config actually exists
    Config config = Config();
    config.LoadFromJson(configFilePath);

    return config;
}



bool DataHandler::WriteConfigFile(QFile &configFile, const Config &config) {
    QDir dir = QFileInfo(configFile).dir();

    // Check if the directory exists, and create it if it doesn't
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {  // mkpath creates the directory and any necessary parent directories
            qWarning() << "Failed to create directories for" << dir.absolutePath();
            return false;
        }
    }

    // Now that the directory exists, create the file
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not create the file:" << configFile.errorString();
        return false;
    }

    QString json(config.GetAsJsonString());
    QTextStream out(&configFile);
    out << json;
    configFile.close();

    qDebug() << "File created successfully at" << dir.absolutePath();

    return true;
}
