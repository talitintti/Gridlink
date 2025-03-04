#ifndef ALBUM_H
#define ALBUM_H
#include <QList>
#include "song.h"
#include <QDebug>
#include <QImage>

class OImage {
public:
    OImage() :
        width_(0),
        height_(0),
        image_format_(QImage::Format_Invalid) {}

    OImage(uint8_t *image_data, uint width, uint height, QImage::Format format) :
        //image_data_(std::shared_ptr<uint8_t>(image_data, std::default_delete<uint8_t[]>())),
        width_(width),
        height_(height),
        image_format_(format)
    {
        // TODO: support other formats (allocated sizes have to be taken into account
        if (format != QImage::Format_RGB888) {
            qCritical() << "USING UNSUPPORTED FORMAT, CHANGE OImage CLASS IMMEDIATELY\n";
        }

        image_data_ = std::shared_ptr<uint8_t>(image_data, std::default_delete<uint8_t[]>());
    }

    uint GetWidth() const { return width_; }
    uint GetHeight() const { return width_; }
    const uint8_t *GetImageData() const { return image_data_.get(); }
    bool HasImage() const { return image_data_ ? true : false; }
    QImage::Format GetFormat() const { return image_format_; }

private:
    std::shared_ptr<uint8_t> image_data_;
    uint width_;
    uint height_;
    QImage::Format image_format_;
};

class Album
{
public:
    Album() = default;
    Album(const Album&) = default;
    Album& operator=(const Album&) = default;
    Album& operator=(Album &&other) = default;

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
    const OImage &GetCover() const;
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
