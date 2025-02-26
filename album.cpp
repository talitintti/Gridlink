#include "album.h"

QString Album::GetName() const {
    return album_name_;
}




const QList<Song> &Album::GetSongs() const {
    return songs_;
}
