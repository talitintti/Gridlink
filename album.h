#ifndef ALBUM_H
#define ALBUM_H
#include <QList>
#include "song.h"
#include <QDebug>
#include <QImage>

class OImage {
public:
    OImage() :
        image_data_(nullptr),
        width_(0),
        height_(0),
        image_format_(QImage::Format_Invalid) {}

    OImage(uint8_t *image_data, uint width, uint height, QImage::Format format) :
        image_data_(image_data),
        width_(width),
        height_(height),
        image_format_(format)
    {
        // TODO: support other formats (allocated sizes have to be taken into account
        if (format != QImage::Format_RGB888)
            qCritical() << "USING UNSUPPORTED FORMAT, CHANGE OImage CLASS IMMEDIATELY\n";
    }

    OImage(const OImage& other)
        : width_(other.width_), height_(other.height_), image_format_(other.image_format_) {
        qWarning() << "We are copying images now\n";
        if (other.image_data_) {
            image_data_ = new uint8_t[width_ * height_ * 3];  // Allocate new memory
            std::memcpy(image_data_, other.image_data_, width_ * height_ * 3); // Copy data
        } else {
            image_data_ = nullptr;
        }
    }
    ~OImage() {
        delete image_data_;
    }

    OImage &operator=(const OImage&) = delete;
    OImage& operator=(OImage &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        image_data_ = other.Release();
        return *this;
    }

    uint GetWidth() const { return width_; }
    uint GetHeight() const { return width_; }
    const uint8_t *GetImageData() const { return image_data_; }
    bool HasImage() const { return image_data_ ? true : false; }
    uint8_t *Release() noexcept {
        uint8_t *temp = image_data_;
        image_data_ = nullptr;
        return temp;
    }

private:
    uint8_t *image_data_;
    uint width_;
    uint height_;
    QImage::Format image_format_;
};

class Album
{
public:
    Album() = default;
    Album(QList<Song> &&songs) : songs_(std::move(songs)) {
        for (const Song &song : std::as_const(songs_)) {
            length_sec_ += song.GetDurationSec();
        }

        if (!songs_.empty()) {
            Song song(songs_.at(0));
            artist_name_ = song.GetArtist();
            album_name_ = song.GetAlbum();
            genre_ = song.GetGenre();
        }
    }

    QString GetName() const;
    const QList<Song> &GetSongs() const;
    const uint8_t *GetCoverData() const;
    bool HasCoverData() const;
    void SetCoverData(OImage &&image);

private:
    QList<Song> songs_;
    QString album_name_;
    unsigned length_sec_ = 0;
    QString artist_name_;
    QString genre_;
    OImage cover_image_;
};

#endif // ALBUM_H
