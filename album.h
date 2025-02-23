#ifndef ALBUM_H
#define ALBUM_H
#include <QList>
#include "song.h"
#include <QDebug>

class Album
{
public:
    Album(QList<Song> &&songs) : songs_(std::move(songs)) {
        //Figure out the length
        for (const Song &song : std::as_const(songs_)) {
            length_sec_ += song.GetLengthSec();
        }

       //Figure out the name, artist and genre
        if (!songs.empty()) {
            Song song(songs_.at(0));
            artist_name_ = song.GetArtist();
            album_name_ = song.GetAlbum();
            genre_ = song.GetGenre();
        }
    }

private:
    QList<Song> songs_;
    unsigned length_sec_;
    QString album_name_;
    QString artist_name_;
    QString genre_;
};

#endif // ALBUM_H
