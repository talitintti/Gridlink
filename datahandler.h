#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "mpd_communication.h"
#include "config.h"
#include <QFile>
#include "album.h"

class DataHandler
{
public:
    DataHandler();
    ~DataHandler();

    // Returns an empty string on success
    // Return error msg on failure
    bool Initialize();
    QList<QString> GetArtistNames();
    QList<QString> GetAlbumsNames(const QString &artist_name);
    QList<Album> GetAlbums(const QString &artist_name);
    Album GetAlbum(const QString &artist_name, const QString &album_name);

private:
    bool WriteConfigFile(QFile &configFile, const Config &config);
    Config ReadConfigFile();

    MPDCommunication mpd_communicator_ = MPDCommunication();
    Config config_;

};

#endif // DATAHANDLER_H
