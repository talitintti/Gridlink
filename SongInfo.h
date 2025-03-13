#ifndef SONGINFO_H
#define SONGINFO_H

#include "song.h"

class SongInfo {
public:
    SongInfo(Song &&song, unsigned elapsed_ms, unsigned kbit_rate) :
    song_(std::move(song)),
    elapsed_ms_(elapsed_ms),
    kbit_rate_(kbit_rate) {}

    Song song_;
    unsigned elapsed_ms_;
    unsigned kbit_rate_;
};


#endif // SONGINFO_H
