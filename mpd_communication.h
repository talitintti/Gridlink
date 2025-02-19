#ifndef MPD_COMMUNICATION_H
#define MPD_COMMUNICATION_H

#include <QString>
#include <QList>

#include <mpd/client.h>

class MPDCommunication
{
public:
    MPDCommunication();
    ~MPDCommunication();

    // Returns true if success
    // Return false if no success
    bool Initialize();

    QList<QString> GetArtists(QString artist_type);
    QList<QString> GetAlbumsOfAnArtist(QString artist_name);
private:
    QList<QString> GetTags(const char *return_tag, const char *constraint_tag, const char *constraint_val);
    struct mpd_connection* conn_;
};

#endif // MPD_COMMUNICATION_H
