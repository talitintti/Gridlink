#include "mpdstatus.h"

// Methods with null checks
int MPDStatus::Volume() const { return status_ ? mpd_status_get_volume(status_) : -1; }
int MPDStatus::Random() const { return status_ ? mpd_status_get_random(status_) : -1; }
int MPDStatus::Repeat() const { return status_ ? mpd_status_get_repeat(status_) : -1; }
mpd_consume_state MPDStatus::Consume() const { return status_ ? mpd_status_get_consume_state(status_) : MPD_CONSUME_OFF; }
unsigned MPDStatus::ElapsedTimeMs() const { return status_ ? mpd_status_get_elapsed_ms(status_) : 0; }
unsigned MPDStatus::TotalTimeS() const { return status_ ? mpd_status_get_total_time(status_) : 0; }
unsigned MPDStatus::KbitRate() const { return status_ ? mpd_status_get_kbit_rate(status_) : 0; }
mpd_state MPDStatus::State() const { return status_ ? mpd_status_get_state(status_) : MPD_STATE_UNKNOWN; }
bool MPDStatus::Updating() const { return status_ ? mpd_status_get_update_id(status_) != 0 : false; }

AudioFormat MPDStatus::GetAudioFormat() const {
    struct AudioFormat audio_format;

    if (status_ == NULL) {
        return audio_format;
    }

    auto mpd_format = mpd_status_get_audio_format(status_);

    audio_format.sample_rate = mpd_format->sample_rate;
    audio_format.bits = mpd_format->bits;
    audio_format.channels = mpd_format->channels;

    return audio_format;
}

const Song &MPDStatus::CurrentSong() const {
    return current_song_;
}

bool MPDStatus::IsEmpty() const {
    return status_ == nullptr;
}

bool MPDStatus::SongChanged() const {
    return song_changed_;
}
