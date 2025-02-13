#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "mpd_communication.h"

class DataHandler
{
public:
    DataHandler();
    ~DataHandler();

    QString Initialize();
    QList<QString> GetArtistNames();

private:

    MPDCommunication mpd_communicator = MPDCommunication();

};

#endif // DATAHANDLER_H
