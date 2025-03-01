#include "album.h"

QString Album::GetName() const {
    return album_name_;
}




const QList<Song> &Album::GetSongs() const {
    return songs_;
}

const uint8_t *Album::GetCoverData() const {
    return cover_data_.data();
}

void Album::SetCoverData(std::vector<uint8_t> &&cover_data) {
    cover_data_ = std::move(cover_data);
}
