#ifndef SONGCOLLECTION_H
#define SONGCOLLECTION_H

#include <QList>
#include "song.h"
#include "enums.h"
#include <QDebug>
#include <QImage>


class SongCollection
{
public:
    SongCollection() = default;
    SongCollection(QList<Song> &&songs, const QString &name);
    SongCollection(const SongCollection&) = default;
    SongCollection& operator=(const SongCollection&) = default;
    SongCollection& operator=(SongCollection &&other) = default;

    QString GetName() const;
    time_t LastModified() const;
    const QList<Song> &GetSongs() const;
    const uint8_t *GetCoverData() const;
    const QImage &GetCover() const;
    bool HasCoverData() const;
    void AddSong(Song &&song);
    void SetCoverData(QImage &&img);
    size_t GetHash() const;
    virtual SONGCOLLECTION_TYPE Identify() const;

    // Gets the mtime of the most recently modified song file in this collection
    // Can be used for sorting by last modified
    time_t GetLastModifFile() const;

protected:
    QList<Song> songs_;
    QString name_;
    unsigned length_sec_ = 0;
    QImage cover_image_;
    time_t last_modified_song_time_ = 0;
};

#endif // SONGCOLLECTION_H
