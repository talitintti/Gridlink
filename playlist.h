#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "songcollection.h"

class Playlist : public SongCollection {
public:
    using SongCollection::SongCollection;

    Playlist(QList<Song> &&songs, const QString &name, const QString &path, time_t last_modified_posix) :
        SongCollection(std::move(songs), name),
        path_(path),
        last_modified_posix_(last_modified_posix) {}

    time_t LastModified() { return last_modified_posix_; }

private:
    QString path_;
    time_t last_modified_posix_;
};

#endif // PLAYLIST_H
