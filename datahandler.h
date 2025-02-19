#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "mpd_communication.h"
#include "config.h"
#include <QFile>

class DataHandler
{
public:
    DataHandler();
    ~DataHandler();

    // Returns an empty string on success
    // Return error msg on failure
    bool Initialize();
    QList<QString> GetArtistNames();
    QList<QString> GetAlbumsForArtist(const QString &artist_name);

private:
    bool WriteConfigFile(QFile &configFile, const Config &config);
    Config ReadConfigFile();

    MPDCommunication mpd_communicator_ = MPDCommunication();
    Config config_;

};

#endif // DATAHANDLER_H
