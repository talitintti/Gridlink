#include "album.h"

QString Album::GetAlbumArtist() {
    return songs_.empty() ? "" : songs_.first().GetAlbumArtist();
}

SONGCOLLECTION_TYPE Album::Identify() const {
    return SONGCOLLECTION_TYPE::ALBUM;
}
