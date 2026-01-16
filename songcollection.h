#ifndef SONGCOLLECTION_H
#define SONGCOLLECTION_H

#include <QList>
#include "song.h"
#include "enums.h"
#include <QDebug>
#include <QImage>


class OImage {
public:
    OImage() :
        width_(0),
        height_(0),
        bytes_per_line_(0),
        image_format_(QImage::Format_Invalid) {}

    OImage(std::shared_ptr<uint8_t[]> ptr, uint width, uint height, uint bytes_per_line, QImage::Format format) :
        image_data_(ptr),
        width_(width),
        height_(height),
        bytes_per_line_(bytes_per_line),
        image_format_(format)
    {
        // TODO: support other formats (allocated sizes have to be taken into account
        if (format != QImage::Format_RGB888) {
            qCritical() << "USING UNSUPPORTED FORMAT, CHANGE OImage CLASS IMMEDIATELY\n";
        }
    }

    uint GetWidth() const { return width_; }
    uint GetHeight() const { return width_; }
    uint GetBytesPerLine() const { return bytes_per_line_; }
    const uint8_t *GetImageData() const { return image_data_.get(); }
    bool HasImage() const { return image_data_ ? true : false; }
    QImage::Format GetFormat() const { return image_format_; }

private:
    std::shared_ptr<uint8_t[]> image_data_;
    uint width_;
    uint height_;
    uint bytes_per_line_;
    QImage::Format image_format_;
};

class SongCollection
{
public:
    SongCollection() = default;
    SongCollection(const SongCollection&) = default;
    SongCollection& operator=(const SongCollection&) = default;
    SongCollection& operator=(SongCollection &&other) = default;

    SongCollection(QList<Song> &&songs, const QString &name) :
        songs_(std::move(songs)),
        name_(name)
    {
        for (const Song &song : std::as_const(songs_)) {
            length_sec_ += song.GetDurationSec();
        }
    }

    QString GetName() const;
    const QList<Song> &GetSongs() const;
    const uint8_t *GetCoverData() const;
    const QImage &GetCover() const;
    bool HasCoverData() const;
    void AddSong(Song &&song);
    void SetCoverData(QImage &&img);
    size_t GetHash() const;
    virtual SONGCOLLECTION_TYPE Identify() const;


protected:
    QList<Song> songs_;
    QString name_;
    unsigned length_sec_ = 0;
    QImage cover_image_;
};

#endif // SONGCOLLECTION_H
