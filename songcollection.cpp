#include "songcollection.h"

SongCollection::SongCollection(QList<Song> &&songs, const QString &name) :
        songs_(std::move(songs)),
        name_(name)
    {
        time_t most_recent_mtime = 0;
        for (const Song &song : std::as_const(songs_)) {
            length_sec_ += song.GetDurationSec();

            auto last_modif = song.GetLastModified();
            if (last_modif > most_recent_mtime)
                most_recent_mtime = last_modif;
        }

        last_modified_song_time_ = most_recent_mtime;
    }


void SongCollection::AddSong(Song &&song) {
    length_sec_ += song.GetDurationSec();
    songs_.push_back(std::move(song));
}

QString SongCollection::GetName() const {
    return name_;
}

const QList<Song> &SongCollection::GetSongs() const {
    return songs_;
}

const uint8_t *SongCollection::GetCoverData() const {
    return cover_image_.bits();
}

void SongCollection::SetCoverData(QImage &&qimage) {
    cover_image_ = std::move(qimage);
}

bool SongCollection::HasCoverData() const {
    return !cover_image_.isNull();
}

const QImage &SongCollection::GetCover() const {
    return cover_image_;
}

SONGCOLLECTION_TYPE SongCollection::Identify() const {
    return SONGCOLLECTION_TYPE::SONGCOLLECTION;
}

size_t SongCollection::GetHash() const {
    std::size_t h1 = std::hash<unsigned>()(this->length_sec_);
    std::size_t h2 = std::hash<QString>()(this->name_);
    return h1 ^ (h2 << 1);
}

time_t SongCollection::GetLastModifFile() const {
    return last_modified_song_time_;
}
