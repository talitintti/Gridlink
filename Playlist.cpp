#include "playlist.h"

time_t Playlist::LastModified() const {
    return last_modified_posix_;
}

songcollection_type Playlist::Identify() const {
    return PLAYLIST;
}
