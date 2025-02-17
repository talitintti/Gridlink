#ifndef SONG_H
#define SONG_H

#include <mpd/client.h>
#include <memory>
#include <QString>

class Song
{
public:
    Song();
    Song(mpd_song *mpdSong);

    Song(const Song &cop) : mpdSong(cop.mpdSong) { }
    Song(Song &&mv) : mpdSong(std::move(mv.mpdSong)) { }
    Song &operator=(Song assn)
    {
        mpdSong = std::move(assn.mpdSong);
        return *this;
    }

    QString GetName() const;
    QString GetArtist() const;
    QString GetAlbumArtist() const;
    QString GetAlbum() const;
    unsigned GetLengthSec() const;

private:
    std::shared_ptr<mpd_song> mpdSong;

    const char* GetTagStr(mpd_tag_type) const;
};

#endif // SONG_H
