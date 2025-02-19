#include "song.h"

Song::Song(mpd_song *mpdSong) {
    this->mpd_song_ = std::shared_ptr<mpd_song>(mpdSong, mpd_song_free);
}

const char* Song::GetTagStr(mpd_tag_type tag_type) const {
    if (mpd_song_.get() == nullptr) {
        return "";
    }

    return mpd_song_get_tag(mpd_song_.get(), tag_type, 0);
}

QString Song::GetName() const {
    auto tag = GetTagStr(MPD_TAG_NAME);
    return QString(tag);
}

QString Song::GetArtist() const {
    auto tag = GetTagStr(MPD_TAG_ARTIST);
    return QString(tag);
}

QString Song::GetAlbumArtist() const {
    auto tag = GetTagStr(MPD_TAG_ALBUM_ARTIST);
    return QString(tag);
}

QString Song::GetAlbum() const {
    auto tag = GetTagStr(MPD_TAG_ALBUM);
    return QString(tag);
}

unsigned Song::GetLengthSec() const {
    return mpd_song_get_duration(mpd_song_.get());
}
