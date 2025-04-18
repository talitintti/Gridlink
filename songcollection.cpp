#include "songcollection.h"

void SongCollection::AddSong(Song &&song) {
    length_sec_ += song.GetDurationSec();
    songs_.push_back(std::move(song));
}

QString SongCollection::GetName() const {
    return name_;
}

const QList<Song> &SongCollection::GetSongs() const {
    return songs_;
}

const uint8_t *SongCollection::GetCoverData() const {
    return cover_image_.GetImageData();
}

void SongCollection::SetCoverData(OImage &&image) {
    cover_image_ = std::move(image);
}

bool SongCollection::HasCoverData() const {
    return cover_image_.HasImage();
}

const OImage &SongCollection::GetCover() const {
    return cover_image_;
}

SONGCOLLECTION_TYPE SongCollection::Identify() const {
    return SONGCOLLECTION_TYPE::SONGCOLLECTION;
}

size_t SongCollection::GetHash() const {
    std::size_t h1 = std::hash<unsigned>()(this->length_sec_);
    std::size_t h2 = std::hash<QString>()(this->name_); // Example: hashing song duration
    return h1 ^ (h2 << 1); // Bitwise XOR + shift for better distribution
}
