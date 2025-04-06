#include "album.h"
//
//QString Album::GetName() const {
//    return album_name_;
//}
//
//const QList<Song> &Album::GetSongs() const {
//    return songs_;
//}
//
//const uint8_t *Album::GetCoverData() const {
//    return cover_image_.GetImageData();
//}
//
//void Album::SetCoverData(OImage &&image) {
//    cover_image_ = std::move(image);
//}
//
//bool Album::HasCoverData() const {
//    return cover_image_.HasImage();
//}
//
//const OImage &Album::GetCover() const {
//    return cover_image_;
//}


QString Album::GetAlbumArtist() {
    return songs_.empty() ? "" : songs_.first().GetAlbumArtist();
}

songcollection_type Album::Identify() const {
    return ALBUM;
}
