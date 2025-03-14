#ifndef MPD_COMMUNICATION_H
#define MPD_COMMUNICATION_H

#include <QString>
#include <QList>
#include "song.h"

#include <mpd/client.h>

struct AudioFormat {
   uint32_t sample_rate = 0;
   uint8_t bits = 0;
   uint8_t channels = 0;
};

class MPDStatus {

public:
    MPDStatus(struct mpd_status *status) :
        status_(status) {}
    ~MPDStatus() {
        mpd_status_free(status_);
    }
    MPDStatus(const MPDStatus&) = delete;
    MPDStatus& operator=(const MPDStatus&) = delete;

    int Volume() const;
    int Random() const;
    int Repeat() const;
    mpd_consume_state Consume() const;
    unsigned ElapsedTime() const;
    unsigned TotalTime() const;
    unsigned KbitRate() const;
    AudioFormat AudioFormat() const;
    enum mpd_state State() const;
    bool Updating() const;

private:
    struct mpd_status *status_;
};

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
    QList<Song> GetSongs(const std::string &artist_name, const std::string &album_name);
    const Song GetCurrentSong();
    unsigned GetKbitRate();
    unsigned ElapsedMS();


private:
    QList<QString> GetTags(const char *return_tag, const char *constraint_tag, const char *constraint_val);
    struct mpd_connection* conn_;
};

#endif // MPD_COMMUNICATION_H
