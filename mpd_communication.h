#ifndef MPD_COMMUNICATION_H
#define MPD_COMMUNICATION_H

#include <QString>
#include <QVector>

#include <mpd/client.h>

class MPDCommunication
{
public:
    MPDCommunication();
    ~MPDCommunication();

    // Returns true if success
    // Return false if no success
    bool Initialize();

    QVector<QString> GetArtists(QString artist_type);
    QVector<QString> GetAlbumsOfAnArtist(QString artist_name);
private:
    struct mpd_connection* conn;
};

#endif // MPD_COMMUNICATION_H
