#ifndef MPD_COMMUNICATION_H
#define MPD_COMMUNICATION_H

#include <QString>
#include <QList>
#include "song.h"

#include <mpd/client.h>


class MPDCommunication
{
public:
    struct Playlist {
        Playlist(const QString &p, time_t t) : path_(p), timestamp_(t) {}

        QString path_;
        time_t timestamp_;
    };

    MPDCommunication();
    ~MPDCommunication();

    // Returns true if success
    // Return false if no success
    bool Initialize();

    QList<mpd_playlist*> GetPlaylistsRaw(); // you are responsible for freeing
    QList<Playlist> GetPlaylists();
    QList<Song> GetPlaylistSongs(const std::string playlist_name);
    QList<QString> GetArtists(const std::string artist_type);
    QList<QString> GetAlbumNames(const std::string artist_name);
    QList<Song> GetSongs(const std::string &artist_name, const std::string &album_name);
    const Song GetCurrentSong();
    unsigned GetKbitRate();
    unsigned ElapsedMS();
    mpd_status *GetStatus();

    void TogglePlay(bool is_playing);
    void SeekPos(unsigned pos_in_queue, unsigned pos_seconds);
    void SetVolume(unsigned volume); // 0-100

    void AddToQueue(const QList<Song> &);
    void PlayInQueue(unsigned index = 0);
    void ClearQueue();
    void AppendToPlaylist(const std::string &playlist_name, const QList<Song> songs);
    void PlayNext();
    void PlayPrevious();

private:
    QList<QString> GetTags(const char *return_tag, const char *constraint_tag, const char *constraint_val);
    struct mpd_connection* conn_;
};

#endif // MPD_COMMUNICATION_H
