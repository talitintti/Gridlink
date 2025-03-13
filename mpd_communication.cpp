#include "mpd_communication.h"
#include <QDebug>
#include "song.h"

MPDCommunication::MPDCommunication() {
}


//TODO: set port as parameter to be loaded from config
bool MPDCommunication::Initialize() {
    conn_ = mpd_connection_new("127.0.0.1", 6800, 0);

    if (conn_ == NULL) {
        qWarning() << "Could not connect to mpd \n";
        return false;
    }

    if (mpd_connection_get_error(conn_) != MPD_ERROR_SUCCESS) {
        qWarning() << "Could not connect to mpd \n";
        auto error_msg = mpd_connection_get_error_message(conn_);
        qWarning() <<  error_msg;
        mpd_connection_free(conn_);
        return false;
    }

    return true;
}

// True on no error; false on error
bool CheckForMPDError(struct mpd_connection *connection) {
    if (mpd_connection_get_error(connection) != MPD_ERROR_SUCCESS) {
        auto error_msg = mpd_connection_get_error_message(connection);
        qWarning() <<  error_msg;
        return false;
    }

    return true;
}

MPDCommunication::~MPDCommunication() {
    mpd_connection_free(conn_);
}

QList<QString> MPDCommunication::GetArtists(const std::string artist_type) {
    QList<QString> found_artists;

    const char* artist_type_c = artist_type.c_str();
    enum mpd_tag_type mpd_tag_type_artist = mpd_tag_name_iparse(artist_type_c);

    mpd_search_db_tags(conn_, mpd_tag_type_artist);
    mpd_search_commit(conn_);
    struct mpd_pair *pair;
    while ((pair = mpd_recv_pair_tag(conn_, mpd_tag_type_artist)) != NULL) {
        found_artists.emplace_back(QString(pair->value));
        mpd_return_pair(conn_, pair);
    }
    mpd_response_finish(conn_);

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


QList<QString> MPDCommunication::GetTags(const char *return_tag, const char *constraint_tag, const char *constraint_val) {
    QList<QString> tag_values;

    enum mpd_tag_type return_tag_type = mpd_tag_name_iparse(return_tag);
    enum mpd_tag_type constraint_tag_type = mpd_tag_name_iparse(constraint_tag);

    mpd_search_db_tags(conn_, return_tag_type);
    mpd_search_add_tag_constraint(conn_,
        MPD_OPERATOR_DEFAULT,
        constraint_tag_type,
        constraint_val
    );
    mpd_search_commit(conn_);

    struct mpd_pair *pair;
    while ((pair = mpd_recv_pair_tag(conn_, return_tag_type)) != NULL) {
        tag_values.emplace_back(QString(pair->value));
        mpd_return_pair(conn_, pair);
    }
    mpd_response_finish(conn_);

    return tag_values;
}


QList<QString> MPDCommunication::GetAlbumNames(const std::string artist_name) {
    const char *artist_tag = "AlbumArtist";
    const char *album_tag = "Album";

    const char *artist_name_c = artist_name.c_str();

    return GetTags(album_tag, artist_tag, artist_name_c);
}


QList<Song> MPDCommunication::GetSongs(const std::string &artist_name, const std::string &album_name) {
    QList<Song> songs;
    enum mpd_tag_type album_type_tag = MPD_TAG_ALBUM;
    enum mpd_tag_type artist_type_tag = MPD_TAG_ALBUM_ARTIST; // could be ARTIST_SORT
    const char *artist_name_c = artist_name.c_str();
    const char *album_name_c = album_name.c_str();
    struct mpd_song *song;

    mpd_search_db_songs(conn_, false);
    mpd_search_add_tag_constraint(conn_,
                                    MPD_OPERATOR_DEFAULT,
                                    album_type_tag,
                                    album_name_c
                                    );
    mpd_search_add_tag_constraint(conn_,
                                    MPD_OPERATOR_DEFAULT,
                                    artist_type_tag,
                                    artist_name_c
                                    );
    mpd_search_commit(conn_);

    while ((song = mpd_recv_song(conn_)) != NULL) {
        struct mpd_song *duplicate = mpd_song_dup(song);
        songs.emplace_back(duplicate);
        mpd_song_free(song);
    }
    mpd_response_finish(conn_);

    return songs;
}

const Song MPDCommunication::GetCurrentSong() {
    struct mpd_song *song;

    if (!mpd_send_current_song(conn_)) {
        qWarning() << "Failed to send MPD status request\n";
        CheckForMPDError(conn_);
        return Song();
    }

    if ((song = mpd_recv_song(conn_)) == NULL) {
        qWarning() << "Failed to receive MPD song \n";
        CheckForMPDError(conn_);
        return Song();
    }

    struct mpd_song *duplicate = mpd_song_dup(song);
    mpd_song_free(song);
    mpd_response_finish(conn_);

    return Song(duplicate);
}

unsigned MPDCommunication::GetKbitRate() {            // Send status request asynchronously
    if (!mpd_send_status(conn_)) {
        CheckForMPDError(conn_);
        qWarning() << "Failed to send MPD status request\n";
        return 0;
    }

    // Receive response
    struct mpd_status *status = mpd_recv_status(conn_);
    unsigned kbit_rate = mpd_status_get_kbit_rate(status);

    mpd_status_free(status);
    mpd_response_finish(conn_);

    return kbit_rate;
}

unsigned MPDCommunication::ElapsedMS() {
    if (!mpd_send_status(conn_)) {
        CheckForMPDError(conn_);
        qWarning() << "Failed to send MPD status request\n";
        return 0;
    }

    // Receive response
    struct mpd_status *status = mpd_recv_status(conn_);
    unsigned elapsed_ms = mpd_status_get_elapsed_ms(status);

    mpd_status_free(status);
    mpd_response_finish(conn_);

    return elapsed_ms;

}
