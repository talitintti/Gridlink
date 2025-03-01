#ifndef ALBUM_H
#define ALBUM_H
#include <QList>
#include "song.h"
#include <QDebug>

class Album
{
public:
    Album() = default;
    Album(QList<Song> &&songs) : songs_(std::move(songs)) {
        for (const Song &song : std::as_const(songs_)) {
            length_sec_ += song.GetDurationSec();
        }

        if (!songs_.empty()) {
            Song song(songs_.at(0));
            artist_name_ = song.GetArtist();
            album_name_ = song.GetAlbum();
            genre_ = song.GetGenre();
        }
    }
    QString GetName() const;
    const QList<Song> &GetSongs() const;
    const uint8_t *GetCoverData() const;
    void SetCoverData(std::vector<uint8_t> &&cover_data);
private:
    QList<Song> songs_;
    QString album_name_;
    unsigned length_sec_ = 0;
    QString artist_name_;
    QString genre_;
    std::vector<uint8_t> cover_data_;
};

#endif // ALBUM_H
