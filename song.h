#ifndef SONG_H
#define SONG_H

#include <mpd/client.h>
#include <memory>
#include <QString>

class Song
{
public:
    Song() = default;
    Song(mpd_song *mpdSong);

    Song(const Song &cop) : mpd_song_(cop.mpd_song_) { }
    Song(Song &&mv) : mpd_song_(std::move(mv.mpd_song_)) { }
    Song &operator=(Song assn)
    {
        mpd_song_ = std::move(assn.mpd_song_);
        return *this;
    }

    bool operator==(const Song &other) const {
        return this->GetSongPath() == other.GetSongPath();
    }

    QString GetName() const;
    QString GetArtist() const;
    QString GetAlbumArtist() const;
    QString GetAlbum() const;
    QString GetGenre() const;
    QString GetSongPath() const;
    unsigned GetDurationSec() const;


private:
    std::shared_ptr<mpd_song> mpd_song_;

    const char* GetTagStr(mpd_tag_type) const;
};

#endif // SONG_H
