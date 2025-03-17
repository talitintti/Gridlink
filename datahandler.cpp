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


bool DataHandler::Initialize() {
    config_ = ReadConfigFile();

    if (!mpd_communicator_.Initialize()) {
        return false;
    }

    // Initialize timer for updating the UI through signals
    timer_ = new QTimer(this);
    connect(timer_,
            &QTimer::timeout,
            this,
            &DataHandler::SongInfoUpdate);

    //Make a thread to get mpd notifications about status updates
    mpd_status_updates_ = new MPDNotif(this);
    if (!mpd_status_updates_->MakeConnection()) { //TODO:read from config file to get mpd settings
        return false;
    }

    connect(mpd_status_updates_,
            &MPDNotif::MpdNotifSignal,
            this,
            &DataHandler::StatusUpdateSlot);
    mpd_status_updates_->start();

    return true;
}

void DataHandler::SongInfoUpdate() {
    auto mpd_status = mpd_communicator_.GetStatus();
    if (mpd_status == NULL || mpd_status_get_state(mpd_status) == MPD_STATE_STOP) {
        return emit StatusUpdateSignal(last_update_);
    }

    Song song = std::move(mpd_communicator_.GetCurrentSong());
    MPDStatus status(mpd_status, std::move(song));
    last_update_ = std::move(status);

    emit StatusUpdateSignal(last_update_);
}

// Starts the timer for periodic signals
void DataHandler::StartTimer(unsigned timeout_msec) {
    if (timeout_msec > INT_MAX) timeout_msec = INT_MAX;
    timer_->start(timeout_msec);
}

void DataHandler::StopTimer() {
    timer_->stop();
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


    return Album(std::move(songs));
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
        Album album(std::move(mpd_communicator_.GetSongs(ar_name_std, al_name_std)));
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

//TODO: implement all
void DataHandler::StatusUpdateSlot(mpd_idle events) {
    if (events & MPD_IDLE_DATABASE) {
        std::cout << "Database updated!\n";
    }
    if (events & MPD_IDLE_UPDATE) {
        std::cout << "Update started!\n";
    }
    if (events & MPD_IDLE_STORED_PLAYLIST) {
        std::cout << "Stored playlist changed!\n";
    }
    if (events & MPD_IDLE_PLAYLIST) {
        std::cout << "Playlist changed!\n";
    }
    if (events & MPD_IDLE_PLAYER) {
        std::cout << "Player state changed!\n";
    }
    if (events & MPD_IDLE_MIXER) {
        std::cout << "Mixer settings changed!\n";
    }
    if (events & MPD_IDLE_OUTPUT) {
        std::cout << "Output settings changed!\n";
    }
    if (events & MPD_IDLE_OPTIONS) {
        std::cout << "Options changed!\n";
    }
    if (events & MPD_IDLE_PARTITION) {
        std::cout << "Partition changed!\n";
    }
    if (events & MPD_IDLE_STICKER) {
        std::cout << "Sticker database changed!\n";
    }
    if (events & MPD_IDLE_SUBSCRIPTION) {
        std::cout << "Subscription changed!\n";
    }
    if (events & MPD_IDLE_MESSAGE) {
        std::cout << "New message received!\n";
    }
}
