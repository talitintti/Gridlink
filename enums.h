#ifndef ENUMS_H
#define ENUMS_H

// This defines the current view AND
// the index within left side selected
// clickable views
enum class VIEW {
    HOME,
    LIBRARY,
    ARTIST,
    ALBUM,
    SEARCH,
    PLAYLIST,
    DISCOVER,
    UNKNOWN
};

//// Simply associates left side buttons (home, library, discover etc with index in the list)
//// TODO: this is nonsense, group the data properly with the view or something
//constexpr int BUTTON_INDEXES[5][2] = {
//    { (int)VIEW::HOME, 0 },
//    { (int)VIEW::LIBRARY, 1},
//    { (int)VIEW::ARTIST, 1},
//    { (int)VIEW::ALBUM, 1},
//    { (int)VIEW::DISCOVER, 2},
//};

enum class SONGCOLLECTION_TYPE {
    SONGCOLLECTION,
    PLAYLIST,
    ALBUM
};



#endif // ENUMS_H
