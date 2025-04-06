#include "datahandler.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

std::shared_ptr<uint8_t[]> GetPicture(const std::string &path, int &width, int &height); // TODO: maybe move this function to other file instead

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

QList<Playlist> &DataHandler::GetPlaylists() {
    FetchPlaylists();
    return playlists_;
}

void DataHandler::FetchPlaylists() {
    QList<Playlist> ready_playlists;
    auto mpd_playlists = mpd_communicator_.GetPlaylists();

    for (const auto &mpd_playlist : std::as_const(mpd_playlists)) {
        auto path = mpd_playlist.path_;
        auto filename = QFileInfo(path).fileName();
        auto songs = mpd_communicator_.GetPlaylistSongs(filename.toStdString());
        auto modif_time = mpd_playlist.timestamp_;

        ready_playlists.emplace_back(
            std::move(songs),
            filename,
            path,
            modif_time
        );
    }

    playlists_ = std::move(ready_playlists);
}

const Playlist &DataHandler::GetPlaylist(uint row) {
    return playlists_.at(row);
}


QList<QString> DataHandler::GetArtistNames() {
    return mpd_communicator_.GetArtists("AlbumArtist"); // maybe config file decides artist_type
}


QList<QString> DataHandler::GetAlbumNames(const QString &artist_name) {
    auto list = mpd_communicator_.GetAlbumNames(artist_name.toStdString());
    return list;
}


Album DataHandler::GetAlbum(const QString &artist_name, const QString &album_name) {
    QList<Song> songs;
    std::string ar_name_std = artist_name.toStdString();
    std::string al_name_std = album_name.toStdString();
    songs = mpd_communicator_.GetSongs(ar_name_std, al_name_std);

    return Album(std::move(songs), album_name);
}


void DataHandler::SetAlbumCover(Album &album) const {
    int width, height;
    QString base_path = config_.GetMusicDir();
    QString song_path = album.GetSongs().at(0).GetSongPath();
    QDir music_dir(base_path);
    const std::string full_path = music_dir.filePath(song_path).toStdString();

    //auto picture_data = GetPicture(full_path, width, height);
    std::shared_ptr<uint8_t> picture_data;
    if (picture_data) {
        OImage image(picture_data, width, height, QImage::Format_RGB888);
        album.SetCoverData(std::move(image));
    }
}


QList<Album> DataHandler::GetAlbums(const QString &artist_name) {
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
        FetchPlaylists();
        emit PlaylistsChanged();
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


void DataHandler::AddToPlaylist(const QList<Song> &list, const Playlist &playlist) {
    mpd_communicator_.AppendToPlaylist(playlist.GetName().toStdString(), list);
}

std::shared_ptr<uint8_t[]> GetPicture(const std::string& filename, int& width, int& height) {
    auto destination_format = AV_PIX_FMT_RGB24;
    // Open the input file
    AVFormatContext* format_ctx = nullptr;
    if (avformat_open_input(&format_ctx, filename.c_str(), nullptr, nullptr) != 0) {
        qWarning() << ("Could not open file: " + filename);
        return nullptr;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not find stream information.");
        return nullptr;
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
        return nullptr;
    }

    // Get the attached picture packet
    AVPacket& packet = format_ctx->streams[image_stream_index]->attached_pic;

    // Find the decoder for the image
    AVCodecParameters* codec_params = format_ctx->streams[image_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
    if (!codec) {
        avformat_close_input(&format_ctx);
        qWarning() << ("Unsupported image codec.");
        return nullptr;
    }

    // Allocate a codec context
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not allocate codec context.");
        return nullptr;
    }

    // Copy codec parameters to the codec context
    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not copy codec parameters to context.");
        return nullptr;
    }

    // Open the codec
    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not open codec.");
        return nullptr;
    }

    // Allocate a frame for decoding
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not allocate frame.");
        return nullptr;
    }

    // Send the packet to the decoder
    if (avcodec_send_packet(codec_ctx, &packet) < 0) {
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Failed to send packet to decoder.");
        return nullptr;
    }

    // Receive the decoded frame
    if (avcodec_receive_frame(codec_ctx, frame) < 0) {
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Failed to receive frame from decoder.");
        return nullptr;
    }

    // Get image dimensions
    width = frame->width;
    height = frame->height;

    // Convert the frame to RGB24 format
    SwsContext* sws_ctx = sws_getContext(
    width, height, codec_ctx->pix_fmt, // Source format
    width, height, destination_format,  // Destination format
    SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!sws_ctx) {
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        qWarning() << ("Could not initialize SwsContext.");
        return nullptr;
    }

    uint8_t *dst_data[4] = { nullptr };
    int dest_linesize[4] = { 0 };

    int buffer_size = av_image_alloc(dst_data, dest_linesize, width, height, destination_format, 1);
    if (buffer_size < 0) {
        qWarning() << "Error: Couldn't allocate destination buffer.";
        sws_freeContext(sws_ctx);
        av_frame_free(&frame);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&format_ctx);
        return nullptr;
    }
    sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, dst_data, dest_linesize);

    // Clean up
    sws_freeContext(sws_ctx);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);

    // Create shared_ptr that will automatically call av_freep on `data[0]` when it goes out of scope
    // Create a shared_ptr with a lambda deleter
    std::shared_ptr<uint8_t[]> result(dst_data[0], [](uint8_t* ptr) {
        av_freep(&ptr);  // Deleter to free the memory with av_freep
    });

    return result;
}
