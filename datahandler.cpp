#include "datahandler.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <iostream>
#include <QSharedPointer>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
}


typedef struct {
    uint8_t *data_plane_ptrs[4];
    int dest_linesizes[4];
} image_t;

image_t GetPicture(const std::string& filename, int& width, int& height,
                                      AVPixelFormat destination_format, uint8_t padding) {
    image_t ret;
    ret = {};

    // Open the input file
    AVFormatContext* format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, filename.c_str(), nullptr, nullptr) != 0) {
        qWarning() << ("Could not open file: " + filename);
        return ret;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not find stream information.");
        return ret;
    }

    // Find the attached picture stream
    int image_stream_index = -1;
    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
            image_stream_index = i;
            break;
        }
    }

    if (image_stream_index == -1) {
        avformat_close_input(&format_ctx);
        qWarning() << ("No embedded image found in the file.");
        return ret;
    }

    // Get the attached picture packet
    AVPacket& packet = format_ctx->streams[image_stream_index]->attached_pic;

    // Find the decoder for the image
    AVCodecParameters* codec_params = format_ctx->streams[image_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        avformat_close_input(&format_ctx);
        qWarning() << ("Unsupported image codec.");
        return ret;
    }

    // Allocate a codec context
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not allocate codec context.");
        return ret;
    }

    // Copy codec parameters to the codec context
    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not copy codec parameters to context.");
        return ret;
    }

    // Open the codec
    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not open codec.");
        return ret;
    }

    // Allocate a frame for decoding
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not allocate frame.");
        return ret;
    }

    // Send the packet to the decoder
    if (avcodec_send_packet(codec_ctx, &packet) < 0) {
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Failed to send packet to decoder.");
        return ret;
    }

    // Receive the decoded frame
    if (avcodec_receive_frame(codec_ctx, frame) < 0) {
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Failed to receive frame from decoder.");
        return ret;
    }

    // Get image dimensions
    width = frame->width;
    height = frame->height;


    // Convert the frame to RGB24 format
    SwsContext* sws_ctx = sws_getContext(
                    width, height, static_cast<AVPixelFormat>(frame->format), // Source format
                    width, height, destination_format,  // Destination format
                    SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!sws_ctx) {
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not initialize SwsContext.");
        return ret;
    }

    uint8_t *dst_data[4] = { nullptr, nullptr, nullptr, nullptr };
    int dest_linesize[4] = { 0, 0, 0, 0 };

    int buffer_size = av_image_alloc(dst_data, dest_linesize, width, height,
                                     destination_format, padding);
    if (buffer_size < 0) {
        qWarning() << "Error: Couldn't allocate destination buffer.";
        sws_freeContext(sws_ctx);
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        ret.data_plane_ptrs[0] = nullptr;
        return ret;
    }

    sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, dst_data, dest_linesize);

    // Clean up
    sws_freeContext(sws_ctx);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);

    ret.data_plane_ptrs[0] = dst_data[0];
    ret.data_plane_ptrs[1] = dst_data[1];
    ret.data_plane_ptrs[2] = dst_data[2];
    ret.data_plane_ptrs[3] = dst_data[3];

    ret.dest_linesizes[0] = dest_linesize[0];
    ret.dest_linesizes[1] = dest_linesize[1];
    ret.dest_linesizes[2] = dest_linesize[2];
    ret.dest_linesizes[3] = dest_linesize[3];

    return ret;
}

DataHandler::~DataHandler() {
}

bool DataHandler::Initialize() {
    config_ = ReadConfigFile();

    if (!mpd_communicator_.Initialize()) {
        return false;
    }

    // Let's get updates from MPD on MPD server state changes
    mpd_status_updates_ = new MPDNotif(this, NULL, 0, 1000);
    mpd_status_updates_->SetBlockingTime(500);
    connect(mpd_status_updates_,
            &MPDNotif::PlayerStateChanged,
            this,
            &DataHandler::StatusUpdateSlot);

    return true;
}

// Updates the DataHandler status and sends the signal
bool DataHandler::FetchStatusUpdate() {
    auto mpd_status = mpd_communicator_.GetStatus();

    if (mpd_status == nullptr) return false;

    bool song_changed = false;
    Song current_song = std::move(mpd_communicator_.GetCurrentSong());
    size_t current_song_hash = current_song.GetHash();
    size_t last_song_hash = last_update_.CurrentSong().GetHash();

    if (current_song_hash != last_song_hash) song_changed = true;

    MPDStatus status(mpd_status,
                     std::move(current_song),
                     song_changed);
    last_update_ = std::move(status);

    return true;
}

QList<QSharedPointer<Playlist>> *DataHandler::GetPlaylists() {
    FetchPlaylists();
    return &playlists_;
}

void DataHandler::FetchPlaylists() {
    auto mpd_playlists = mpd_communicator_.GetPlaylists();
    if (mpd_playlists.isEmpty()) return;

    playlists_.clear();
    for (const auto &mpd_playlist : std::as_const(mpd_playlists)) {
        auto path = mpd_playlist.path_;
        auto filename = QFileInfo(path).fileName();
        auto songs = mpd_communicator_.GetPlaylistSongs(filename.toStdString());
        auto modif_time = mpd_playlist.timestamp_;

        playlists_.emplace_back(
            QSharedPointer<Playlist>::create(
                std::move(songs),
                filename,
                path,
                modif_time
            )
        );
    }

    //playlists_ = std::move(ready_playlists);
}

const QSharedPointer<Playlist> DataHandler::GetPlaylistH(size_t hash) {
    for (size_t i = 0; i < playlists_.count(); i++) {
        const auto ptr = playlists_.at(i);
        if (ptr->GetHash() == hash) return ptr;
    }
    return {};
}

// Row based search (faster) but you have to know the row
const QSharedPointer<Playlist> DataHandler::GetPlaylistR(unsigned row) {
    if (row >= playlists_.count()) return {};
    return playlists_.at(row);
}


const Playlist *DataHandler::GetPlaylist(unsigned row) {
    if (row >= playlists_.count()) return nullptr;
    return playlists_.at(row).data();
}


QList<QString> DataHandler::GetArtistNames() {
    return mpd_communicator_.GetArtists("AlbumArtist"); // maybe config file decides artist_type
}


QList<QString> DataHandler::GetAlbumNames(const QString &artist_name) {
    auto list = mpd_communicator_.GetAlbumNames(artist_name.toStdString());
    return list;
}


const QSharedPointer<const Album> DataHandler::GetAlbum(const QString &artist_name, const QString &album_name) {
    QList<Song> songs;
    std::string ar_name_std = artist_name.toStdString();
    std::string al_name_std = album_name.toStdString();
    songs = mpd_communicator_.GetSongs(ar_name_std, al_name_std);

    return QSharedPointer<Album>::create(std::move(songs), album_name);
}


void DataHandler::SetAlbumCover(Album &album) const {
    auto &songs = album.GetSongs();
    if (songs.count() == 0) return;

    const AVPixelFormat dst_format = AV_PIX_FMT_RGB24;
    const QImage::Format dst_format_in_qimage = QImage::Format_RGB888;
    const uint align = 32;
    int width = 0, height = 0;
    QString base_path = config_.GetMusicDir();
    QString song_path = songs.at(0).GetSongPath();
    QDir music_dir(base_path);
    const std::string full_path = music_dir.filePath(song_path).toStdString();

    auto pic = GetPicture(full_path, width, height, dst_format, align);

    // Need only the first plane since we use RGB24, change accordingly if you use something else
    auto buffer = pic.data_plane_ptrs[0];
    auto linesize = pic.dest_linesizes[0];
    if (buffer) {
        QImage qimage(
            buffer,
            width,
            height,
            linesize,
            dst_format_in_qimage,
            [](void* ptr) {
                av_free(ptr);
            },
            buffer
        );
        album.SetCoverData(std::move(qimage));
    }
}


QList<Album> DataHandler::GetAlbumsForArtist(const QString &artist_name) {
    QList<Album> albums;
    std::string ar_name_std = artist_name.toStdString();

    QList<QString> album_names(mpd_communicator_.GetAlbumNames(artist_name.toStdString()));
    for (const auto &album_name : std::as_const(album_names)) {
        std::string al_name_std = album_name.toStdString();

        Album album(
            std::move(mpd_communicator_.GetSongs(ar_name_std, al_name_std)),
            album_name);

        SetAlbumCover(album);
        albums.push_back(std::move(album));
    }

    return albums;
}


Config DataHandler::ReadConfigFile() {

    // Figuring out where to config file should go
    QString configFolderPath = QStandardPaths::writableLocation(
        QStandardPaths::ConfigLocation
        );

    // If XDG_DEFAULT_HOME is not set
    if (configFolderPath.isEmpty()) {
        qWarning() << "Defeault config path is not set\n";
        //TODO: signal slot error here
    }

    QString configFilePath = configFolderPath + "/Gridlink/config.json";
    QFile configFile(configFilePath);

    if (!configFile.exists()) {
        Config default_config = Config();
        WriteConfigFile(configFile, default_config);
        return default_config;
    }

    // The config actually exists
    Config config = Config();
    config.LoadFromJson(configFilePath);

    return config;
}



bool DataHandler::WriteConfigFile(QFile &configFile, const Config &config) {
    QDir dir = QFileInfo(configFile).dir();

    // Check if the directory exists, and create it if it doesn't
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {  // mkpath creates the directory and any necessary parent directories
            qWarning() << "Failed to create directories for" << dir.absolutePath();
            return false;
        }
    }

    // Now that the directory exists, create the file
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not create the file:" << configFile.errorString();
        return false;
    }

    QString json(config.GetAsJsonString());
    QTextStream out(&configFile);
    out << json;
    configFile.close();

    qDebug() << "File created successfully at" << dir.absolutePath();

    return true;
}

void DataHandler::ManualStatusUpdate() {
    if (FetchStatusUpdate()) ParseStatusUpdate();
}

void DataHandler::PlaylistChangeHandler() {
    int playlist_count_before = this->playlists_.count();

    QList<size_t> hash_list;
    QList<QString> name_list;
    for (const auto &pl_ptr : std::as_const(playlists_)) { // I'm not using tuples fuck you
        hash_list.append(pl_ptr->GetHash());
        name_list.append(pl_ptr->GetName());
    }

    FetchPlaylists();
    int playlist_count_now = this->playlists_.count();

    // finding the deleted playlists and checking if the playlists have changed
    QList<QString> deleted;
    QList<QSharedPointer<Playlist>> changed;
    for (const auto &new_pl_ptr : std::as_const(playlists_)) {
        size_t curr_hash = new_pl_ptr->GetHash();
        const QString &curr_name = new_pl_ptr->GetName();
        for (size_t i = 0; i < name_list.count(); i++) {
            auto old_name = name_list.at(i);
            auto old_hash = hash_list.at(i);
            if (curr_name == old_name) {
                if (curr_hash != old_hash) changed.append(new_pl_ptr);
                goto iter;
            }
        deleted.append(curr_name);
        iter:
        ;
        }
    }



    if (playlist_count_before > playlist_count_now) {
        emit PlaylistsDeleted(deleted);
    }
    else if (playlist_count_before < playlist_count_now) {
        emit PlaylistAdded();
    }
    else {
        emit PlaylistsChanged(changed);
    }
}


//TODO: implement all
void DataHandler::StatusUpdateSlot(mpd_idle events) {
    if (events & MPD_IDLE_DATABASE) {
        emit DatabaseChanged();
        std::cout << "Database updated! "<< std::endl;
    }
    if (events & MPD_IDLE_UPDATE) {
        /** a database update has started or finished. */
        std::cout << "Update started! "<< std::endl;
    }
    if (events & MPD_IDLE_STORED_PLAYLIST) {
        PlaylistChangeHandler();
        std::cout << "Stored playlist changed! "<< std::endl;
    }
    if (events & MPD_IDLE_PLAYLIST) { // THIS IS QUEUE, NOT "PLAYLIST"
        std::cout << "Playlist changed! "<< std::endl;
    }
    if (events & MPD_IDLE_PLAYER) {
        if (!FetchStatusUpdate()) return;
        ParseStatusUpdate();
        std::cout << "Player state changed! "<< std::endl;
    }
    if (events & MPD_IDLE_MIXER) {
        if (!FetchStatusUpdate()) return;
        ParseStatusUpdate();
        std::cout << "Mixer settings changed! "<< std::endl;
    }
    if (events & MPD_IDLE_OUTPUT) {
        std::cout << "Output settings changed! "<< std::endl;
    }
    if (events & MPD_IDLE_OPTIONS) {
        std::cout << "Options changed! "<< std::endl;
    }
    if (events & MPD_IDLE_PARTITION) {
        std::cout << "Partition changed! "<< std::endl;
    }
    if (events & MPD_IDLE_STICKER) {
        std::cout << "Sticker database changed! "<< std::endl;
    }
    if (events & MPD_IDLE_SUBSCRIPTION) {
        std::cout << "Subscription changed! "<< std::endl;
    }
    if (events & MPD_IDLE_MESSAGE) {
        std::cout << "New message received! "<< std::endl;
    }
}

void DataHandler::ParseStatusUpdate() {
    auto state = last_update_.State();
    auto last_song = last_update_.CurrentSong();
    auto elapsed_ms = last_update_.ElapsedTimeMs();
    auto volume  = last_update_.Volume();

    if (last_update_.SongChanged()) emit SongUpdateSignal(last_song);

    emit SongElapsedSignal(elapsed_ms);
    emit VolumeChanged(volume);

    switch(state) {
        case MPD_STATE_PAUSE:
            emit SongPausedSignal();
            break;
        case MPD_STATE_PLAY:
            emit PlayStartedSignal();
            break;
        case MPD_STATE_STOP:
            emit SongStoppedSignal();
            break;
        case MPD_STATE_UNKNOWN:
            qWarning() << "Unkown state from last status update\n";
            break;
    }

}

void DataHandler::TogglePlay() {
    auto state = last_update_.State();

    switch(state) {
        case MPD_STATE_PAUSE:
            mpd_communicator_.TogglePlay(false);
            break;
        case MPD_STATE_PLAY:
            mpd_communicator_.TogglePlay(true);
            break;
        case MPD_STATE_STOP:
            break;
        case MPD_STATE_UNKNOWN:
            break;
    }
}

void DataHandler::AddToQueue(const QList<Song> &songs) {
    mpd_communicator_.AddToQueue(songs);
}

void DataHandler::StartPlayingQueue(unsigned index) {
    mpd_communicator_.PlayInQueue(index);
}

void DataHandler::ClearQueue() {
    mpd_communicator_.ClearQueue();
}

void DataHandler::SeekPos(unsigned pos_ms) {
    unsigned seconds = round(pos_ms / 1000.0);
    unsigned pos_in_queue = 0; // Going with currently playing
    mpd_communicator_.SeekPos(pos_in_queue, seconds);
}

void DataHandler::SetVolume(unsigned volume) {
    mpd_communicator_.SetVolume(volume);
}

void DataHandler::PlayNext() {
    mpd_communicator_.PlayNext();
}

void DataHandler::PlayPrevious() {
    mpd_communicator_.PlayPrevious();
}


void DataHandler::AddToPlaylist(const QList<Song> &list, const Playlist *playlist) {
    mpd_communicator_.AppendToPlaylist(playlist->GetName().toStdString(), list);
}

void DataHandler::DeletePlaylist(unsigned row) {
    auto name = playlists_.at(row)->GetName();
    std::string name_std = name.toStdString();
    mpd_communicator_.RemovePlaylist(name_std);
    //for (size_t i = 0; i < playlists_.count(); i++) {
    //    auto checking = playlists_.at(i);
    //    if (checking->GetHash() == hash) {
    //        auto std_name = checking->GetName().toStdString();
    //        mpd_communicator_.RemovePlaylist(std_name);
    //        return;
    //    }
    //}
}


void DataHandler::DeleteFromPlaylist(const QList<Song> &songs, const Playlist *playlist) {
    auto playlist_name = playlist->GetName().toStdString();
    int pos_in_playlist = -1;
    for (const auto &song_to_be_delet : std::as_const(songs)) {
        if((pos_in_playlist = playlist->GetSongPosition(song_to_be_delet)) != -1) {
            mpd_communicator_.RemoveFromPlaylist(playlist_name, pos_in_playlist);
        }
    }
}

// True is returned if duplicate or duplicate with (N) is found
// If playlist_name = stored name -> return stored_name(1) \n
// If playlist_name(N) = stored_name where N [0,inf[ -> return playlist_name(N+1) \n
// If playlist_name != stored_name -> return playlist_name
bool HandleName(const QString &stored_name, const QString &playlist_name, QString &new_name) {
        unsigned increment = 0;
        bool had_duplicate = false;
        new_name = QString(playlist_name);

        if (stored_name == playlist_name) {
            increment++;
            new_name = new_name + "(" + QString::number(increment) + ")";

            had_duplicate = true;
        }
        else {
            QRegularExpression re("^" + QRegularExpression::escape(playlist_name) + R"(\(\d+\)$)");
            auto match = re.match(stored_name);
            if (match.hasMatch()) {
                increment = match.captured(1).toInt();
                increment++;
                new_name = new_name + "(" + QString::number(increment) + ")";

                had_duplicate = true;
            }
        }

        return had_duplicate;
}


// Add songs to playlist if name is new we create a new playlist
void DataHandler::CreatePlaylist(const QString &playlist_name , const QList<Song> &songs) {
    QString new_name;

    // Check that there isn't a playlist with the same name already
    for (const auto &playlist : std::as_const(playlists_)) {
        auto stored_name = playlist->GetName();
        if (HandleName(stored_name, playlist_name, new_name)) break;
    }

    // Creating an empty playlist
    auto new_name_std = new_name.toStdString();
    for (const auto &song : songs) {
        auto song_uri = song.GetSongPath().toStdString();
        mpd_communicator_.AddToPlaylist(new_name_std, song_uri);
    }
}


