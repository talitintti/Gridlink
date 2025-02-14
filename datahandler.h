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
    QString Initialize();
    QList<QString> GetArtistNames();

private:
    QString CreateConfigFile(const QFile &configFile);
    Config ReadConfigFile();
    MPDCommunication mpd_communicator = MPDCommunication();

};

#endif // DATAHANDLER_H
