#ifndef MPDSTATUS_H
#define MPDSTATUS_H

#include <mpd/client.h>
#include "song.h"

struct AudioFormat {
   uint32_t sample_rate = 0;
   uint8_t bits = 0;
   uint8_t channels = 0;
};

class MPDStatus {

public:
    MPDStatus() = default;
    MPDStatus(struct mpd_status *status, Song &&current_song) :
        status_(status),
        current_song_(std::move(current_song))    {}

    MPDStatus &operator=(MPDStatus &&other) noexcept {
        current_song_ = std::move(other.current_song_);
        status_ = other.status_;
        other.status_ = nullptr;

        return *this;
    }

    ~MPDStatus() {
        if (status_ != NULL) {
            mpd_status_free(status_);
        }
    }

    MPDStatus(const MPDStatus&) = delete;
    MPDStatus& operator=(const MPDStatus&) = delete;

    int Volume() const;
    int Random() const;
    int Repeat() const;
    mpd_consume_state Consume() const;
    unsigned ElapsedTimeMs() const;
    unsigned TotalTimeS() const;
    unsigned KbitRate() const;
    AudioFormat GetAudioFormat() const;
    enum mpd_state State() const;
    bool Updating() const;
    const Song &CurrentSong() const;

private:
    struct mpd_status *status_ = nullptr;
    Song current_song_;
};

#endif // MPDSTATUS_H
