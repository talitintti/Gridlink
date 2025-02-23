#include "datahandler.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>


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
    return mpd_communicator_.GetArtists("AlbumArtist"); // maybe config file decides artist_type
}

QList<QString> DataHandler::GetAlbumNames(const QString &artist_name) {
    auto list = mpd_communicator_.GetAlbumNames(artist_name.toStdString());
    return list;
}


Album DataHandler::GetAlbum(const QString &artist_name, const QString &album_name) {
    QList<Song> songs;
    std::string ar_name_std = artist_name.toStdString();
    std::string al_name_std = album_name.toStdString();
    songs = mpd_communicator_.GetSongs(ar_name_std, al_name_std);

    Album album(std::move(songs));

    return album;
}

QList<Album> DataHandler::GetAlbums(const QString &artist_name) {
    QList<Album> albums;
    std::string ar_name_std = artist_name.toStdString();

    QList<QString> album_names(mpd_communicator_.GetAlbumNames(artist_name.toStdString()));
    for (const auto &album_name : std::as_const(album_names)) {
        std::string al_name_std = album_name.toStdString();
        albums.emplace_back(mpd_communicator_.GetSongs(ar_name_std,al_name_std));
    }

    return albums;
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
