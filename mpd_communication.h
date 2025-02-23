#ifndef MPD_COMMUNICATION_H
#define MPD_COMMUNICATION_H

#include <QString>
#include <QList>
#include "song.h"

#include <mpd/client.h>

class MPDCommunication
{
public:
    MPDCommunication();
    ~MPDCommunication();

    // Returns true if success
    // Return false if no success
    bool Initialize();

    QList<QString> GetArtists(const std::string artist_type);
    QList<QString> GetAlbumNames(const std::string artist_name);
    void GetSongs(const std::string &artist_name, const std::string &album_name, QList<const Song> &songs);
private:
    QList<QString> GetTags(const char *return_tag, const char *constraint_tag, const char *constraint_val);
    struct mpd_connection* conn_;
};

#endif // MPD_COMMUNICATION_H
