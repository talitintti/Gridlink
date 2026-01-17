#include "mpd_communication.h"
#include <QDebug>
#include "song.h"

MPDCommunication::MPDCommunication() {
}


//TODO: set port as parameter to be loaded from config
bool MPDCommunication::Initialize() {
    conn_ = mpd_connection_new(NULL, 0, 0);

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
    if (connection == NULL) {
        qWarning() << "mpd_connection is null - aborting\n";
        return false;
    }

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

    bool search_db, add_cnstr, commit;
    search_db = mpd_search_db_tags(conn_, return_tag_type);
    add_cnstr = mpd_search_add_tag_constraint(conn_,
        MPD_OPERATOR_DEFAULT,
        constraint_tag_type,
        constraint_val
    );
    commit = mpd_search_commit(conn_);

    if (search_db && add_cnstr && commit) CheckForMPDError(conn_);

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
    enum mpd_tag_type artist_type_tag = MPD_TAG_ALBUM_ARTIST;
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
        qWarning() << "No current song \n";
        CheckForMPDError(conn_);
        return Song();
    }

    struct mpd_song *duplicate = mpd_song_dup(song);
    mpd_song_free(song);
    mpd_response_finish(conn_);

    return Song(duplicate);
}

mpd_status *MPDCommunication::GetStatus() {
    if (!CheckForMPDError(conn_)) return nullptr;

    // Receive response
    struct mpd_status *status = mpd_run_status(conn_);
    if (status == NULL) {
        qWarning() << "Failed to receive MPD status request\n";
        CheckForMPDError(conn_);
        return nullptr;
    }

    return status;
}


unsigned MPDCommunication::GetKbitRate() {
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

// if true: pause, if false: resume
void MPDCommunication::TogglePlay(bool is_playing) {
    mpd_run_pause(conn_, is_playing);
}


void MPDCommunication::AddToQueue(const QList<Song> &song_list) {
    if (!CheckForMPDError(conn_)) {
        qWarning() << "Cannot add to queue \n";
    }

    for (const auto &song : song_list) {
        auto path_std = song.GetSongPath().toStdString();
        if (!mpd_run_add(conn_, path_std.c_str())) {
            qWarning() << "Cannot add song " << path_std << " to queue\n";
            CheckForMPDError(conn_);
        }
    }
}

void MPDCommunication::PlayInQueue(unsigned index_in_queue) {
    if (!mpd_run_play_pos(conn_, index_in_queue)) {
        qWarning() << "Cannot play in queue \n";
        CheckForMPDError(conn_);
    }

    mpd_response_finish(conn_);
}

void MPDCommunication::ClearQueue() {
    if (!CheckForMPDError(conn_)) {
        qWarning() << "Cannot clear queue \n";
    }

    if (!mpd_run_clear(conn_)) {
        CheckForMPDError(conn_);
    }
}

void MPDCommunication::SeekPos(unsigned pos_in_queue, unsigned pos_seconds) {
    if (!CheckForMPDError(conn_)) return;

    if (!mpd_run_seek_pos(conn_, pos_in_queue, pos_seconds)) {
        qWarning() << "Could not seek in song \n";
        CheckForMPDError(conn_);
    }
}

void MPDCommunication::SetVolume(unsigned volume) {
    if (!CheckForMPDError(conn_)) return;

    if (!mpd_run_set_volume(conn_, volume)) {
        qWarning() << "Could not set volume\n";
        CheckForMPDError(conn_);
    }
}

void MPDCommunication::PlayNext() {
    if (!CheckForMPDError(conn_)) return;

    if (!mpd_run_next(conn_)) {
        qWarning() << "Could not play next\n";
        CheckForMPDError(conn_);
    }
}

void MPDCommunication::PlayPrevious() {
    if (!CheckForMPDError(conn_)) return;

    if (!mpd_run_previous(conn_)) {
        qWarning() << "Could not play previous\n";
        CheckForMPDError(conn_);
    }
}

QList<mpd_playlist*> MPDCommunication::GetPlaylistsRaw() {
    QList<mpd_playlist*> playlist_list;
    if (!CheckForMPDError(conn_))  return playlist_list;

    if (!mpd_send_list_playlists(conn_))  {
        qWarning() << "Could not fetch playlists\n";
        CheckForMPDError(conn_);
    }

    mpd_playlist *playlist;
    while ( (playlist = mpd_recv_playlist(conn_)) != NULL) {
        playlist_list.push_back(playlist); // TODO: is it better to duplicate + free old here?
    }

    mpd_response_finish(conn_);

    return playlist_list;
}

QList<MPDCommunication::Playlist> MPDCommunication::GetPlaylists() {
    QList<MPDCommunication::Playlist> playlists;

    auto playlist_pointers = GetPlaylistsRaw();
    for (const auto &playlist_p : playlist_pointers) {
        playlists.push_back(
            MPDCommunication::Playlist(
                mpd_playlist_get_path(playlist_p),
                mpd_playlist_get_last_modified(playlist_p)
            )
        );
        mpd_playlist_free(playlist_p);
    }

    return playlists;
}


QList<Song> MPDCommunication::GetPlaylistSongs(std::string playlist_name) {
    QList<Song> playlist_songs;
    const char *name = playlist_name.c_str();

    if (!CheckForMPDError(conn_))  return playlist_songs;

    if (!mpd_send_list_playlist_meta(conn_, name))  {
        qWarning() << "Could not playlist songs\n";
        CheckForMPDError(conn_);
    }

    mpd_entity *entity;
    while ( (entity = mpd_recv_entity(conn_)) != NULL) {
        const mpd_song *song = mpd_entity_get_song(entity);
        playlist_songs.emplace_back(mpd_song_dup(song));
        mpd_entity_free(entity);
    }

    mpd_response_finish(conn_);

    return playlist_songs;
}

void MPDCommunication::AppendToPlaylist(const std::string &playlist_name, const QList<Song> songs) {
    if (!CheckForMPDError(conn_)) return;

    for (const auto &song : songs) {
        if ( !mpd_run_playlist_add(conn_, playlist_name.c_str(), song.GetSongPath().toStdString().c_str()) )
        {
            qWarning() << "Could not add to playlist\n";
            CheckForMPDError(conn_);
        }
    }

}

void MPDCommunication::RemovePlaylist(std::string &name) {
    if (!CheckForMPDError(conn_)) return;

    if (!mpd_run_rm(conn_, name.c_str())) {
        qWarning() << "Could not delete playlist \n";
        CheckForMPDError(conn_);
    }
}

void MPDCommunication::RemoveFromPlaylist(std::string &playlist_name, uint pos_in_playlist) {
    if (!CheckForMPDError(conn_)) return;

    if (!mpd_run_playlist_delete(conn_, playlist_name.c_str(), pos_in_playlist)) {
        qWarning() << "Could not delete from playlist \n";
        CheckForMPDError(conn_);
    }
}

void MPDCommunication::AddToPlaylist(std::string &playlist_name, std::string &song_path) {
    if (song_path.empty() || !CheckForMPDError(conn_)) return;

    if (!mpd_run_playlist_add(conn_, playlist_name.c_str(), song_path.c_str())) {
        qWarning() << "Could not add to playlist \n Name:" + playlist_name;
        CheckForMPDError(conn_);
    }
}
