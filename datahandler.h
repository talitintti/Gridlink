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

private:
    bool WriteConfigFile(QFile &configFile, const Config &config);


    Config ReadConfigFile();
    MPDCommunication mpd_communicator = MPDCommunication();

    Config config;

};

#endif // DATAHANDLER_H
