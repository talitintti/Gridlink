#ifndef ALBUM_H
#define ALBUM_H
#include <QList>
#include "song.h"

class Album
{
public:
    Album(const QList<Song> &songs) : songs_(std::move(songs)) {
        for (const Song &song : qAsConst(songs_)) {
            length_sec_ += song.GetLengthSec();
        }
    }

private:
    QList<Song> songs_;
    unsigned length_sec_;
};

#endif // ALBUM_H
