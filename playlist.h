#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "songcollection.h"
#include "enums.h"

class Playlist : public SongCollection {
public:
    using SongCollection::SongCollection;

    Playlist(QList<Song> &&songs, const QString &name, const QString &path, time_t last_modified_posix) :
        SongCollection(std::move(songs), name),
        path_(path)
        {}

    SONGCOLLECTION_TYPE Identify() const override;
    int GetSongPosition(const Song &) const;

private:
    QString path_;
};

#endif // PLAYLIST_H
