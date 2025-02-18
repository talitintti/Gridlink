#include "mpd_communication.h"


MPDCommunication::MPDCommunication() {
}


//TODO: set port as parameter to be loaded from config
bool MPDCommunication::Initialize() {
    conn = mpd_connection_new("127.0.0.1", 6800, 0);

    if (conn == NULL) {
        //fprintf(stderr, "Out of memory\n");
        //throw "Out of memory";
        return false;
    }
    if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        //auto error_msg = mpd_connection_get_error_message(conn);
        //fprintf(stderr, "%s\n", error_msg);
        //throw error_msg;
        mpd_connection_free(conn);
        return false;
    }

    return true;

}

MPDCommunication::~MPDCommunication() {
    mpd_connection_free(conn);
}

QList<QString> MPDCommunication::GetArtists(QString artist_type) {
    QList<QString> found_artists;

    const char* artist_type_c = artist_type.toStdString().c_str();
    enum mpd_tag_type mpd_tag_type_artist = mpd_tag_name_iparse(artist_type_c);

    mpd_search_db_tags(conn, mpd_tag_type_artist);
    mpd_search_commit(conn);
    struct mpd_pair *pair;
    while ((pair = mpd_recv_pair_tag(conn, mpd_tag_type_artist)) != NULL) {
        found_artists.push_back(QString(pair->value));
        mpd_return_pair(conn, pair);
    }

    return found_artists;
}

static void
print_tag(const struct mpd_song *song, enum mpd_tag_type type,
      const char *label)
{
    unsigned i = 0;
    const char *value;

    while ((value = mpd_song_get_tag(song, type, i++)) != NULL)
        printf("%s: %s\n", label, value);
}




QList<QString> MPDCommunication::GetAlbumsOfAnArtist(QString artist_name) {
    QList<QString> found_albums;
    const char *tag_name = "AlbumArtist";
    enum mpd_tag_type artist_type = mpd_tag_name_iparse(tag_name);
    const char* artist_name_c = artist_name.toStdString().c_str();

    mpd_search_db_tags(conn, artist_type);
    mpd_search_add_tag_constraint(conn,
        MPD_OPERATOR_DEFAULT,
        artist_type,
        artist_name_c
    );
    mpd_search_commit(conn);

    struct mpd_pair *pair;
    while ((pair = mpd_recv_pair_tag(conn, artist_type)) != NULL) {
        found_albums.push_back(QString(pair->value));
        mpd_return_pair(conn, pair);
    }
    return found_albums;
}

// We need a list of types "song"? These contain everything we need like URL?
// search.h
bool get_album_songs(mpd_connection *conn, const char *album_name, const char *album_artist_name) {
    enum mpd_tag_type album_type_tag = MPD_TAG_ALBUM;
    enum mpd_tag_type artist_type_tag = MPD_TAG_ALBUM_ARTIST; // could be ARTIST_SORT
    struct mpd_song *song;

    mpd_search_db_songs(conn, false);
    mpd_search_add_tag_constraint(conn,
                                    MPD_OPERATOR_DEFAULT,
                                    album_type_tag,
                                    album_name
                                    );
    mpd_search_add_tag_constraint(conn,
                                    MPD_OPERATOR_DEFAULT,
                                    artist_type_tag,
                                    album_artist_name
                                    );
    mpd_search_commit(conn);

    while ((song = mpd_recv_song(conn)) != NULL) {
        printf("uri: %s\n", mpd_song_get_uri(song));
        print_tag(song, MPD_TAG_ARTIST, "artist");
        print_tag(song, MPD_TAG_ALBUM, "album");
        print_tag(song, MPD_TAG_TITLE, "title");
        print_tag(song, MPD_TAG_TRACK, "track");
        print_tag(song, MPD_TAG_NAME, "name");
        print_tag(song, MPD_TAG_DATE, "date");

        if (mpd_song_get_duration(song) > 0) {
            printf("time: %u\n", mpd_song_get_duration(song));
        }

        printf("pos: %u\n", mpd_song_get_pos(song));

        mpd_song_free(song);
    }
    return true;
}

