#include "song.h"
#include <QTime>

const char* Song::GetTagStr(mpd_tag_type tag_type) const {
    if (IsEmpty()) {
        return "";
    }

    return mpd_song_get_tag(mpd_song_.get(), tag_type, 0);
}

QString Song::GetName() const {
    auto tag = GetTagStr(MPD_TAG_TITLE);
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

QString Song::GetGenre() const {
    auto tag = GetTagStr(MPD_TAG_GENRE);
    return QString(tag);
}

unsigned Song::GetDurationSec() const {
    return IsEmpty() ? 0 : mpd_song_get_duration(mpd_song_.get());
}

QString Song::GetSongPath() const {
    return IsEmpty() ? "" : mpd_song_get_uri(mpd_song_.get());
}

std::size_t Song::GetHash() const {
    std::size_t h1 = std::hash<QString>()(this->GetSongPath());
    std::size_t h2 = std::hash<int>()(this->GetDurationSec()); // Example: hashing song duration
    return h1 ^ (h2 << 1); // Bitwise XOR + shift for better distribution
}

bool Song::IsEmpty() const {
    return mpd_song_.get() == nullptr;
}
