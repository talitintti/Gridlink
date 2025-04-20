#include "playlist.h"
#include "enums.h"

time_t Playlist::LastModified() const {
    return last_modified_posix_;
}

SONGCOLLECTION_TYPE Playlist::Identify() const {
    return SONGCOLLECTION_TYPE::PLAYLIST;
}

int Playlist::GetSongPosition(const Song &given_song) const {
    for (int i = 0; i < songs_.count(); i++) {
        auto stored_song = songs_.at(i);
        if (given_song == stored_song) return i;
    }

    return -1;
}
