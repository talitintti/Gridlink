#ifndef ALBUM_H
#define ALBUM_H
#include <QList>
#include "songcollection.h"
#include <QDebug>
#include <QImage>

//class OImage {
//public:
//    OImage() :
//        width_(0),
//        height_(0),
//        image_format_(QImage::Format_Invalid) {}
//
//    OImage(std::shared_ptr<uint8_t> ptr, uint width, uint height, QImage::Format format) :
//        image_data_(ptr),
//        width_(width),
//        height_(height),
//        image_format_(format)
//    {
//        // TODO: support other formats (allocated sizes have to be taken into account
//        if (format != QImage::Format_RGB888) {
//            qCritical() << "USING UNSUPPORTED FORMAT, CHANGE OImage CLASS IMMEDIATELY\n";
//        }
//    }
//
//    uint GetWidth() const { return width_; }
//    uint GetHeight() const { return width_; }
//    const uint8_t *GetImageData() const { return image_data_.get(); }
//    bool HasImage() const { return image_data_ ? true : false; }
//    QImage::Format GetFormat() const { return image_format_; }
//
//private:
//    std::shared_ptr<uint8_t> image_data_;
//    uint width_;
//    uint height_;
//    QImage::Format image_format_;
//};

class Album : public SongCollection
{
public:
    using SongCollection::SongCollection;

    QString GetAlbumArtist();
    songcollection_type Identify() const override;

private:
    //QList<Song> songs_;
    //QString album_name_;
    //unsigned length_sec_ = 0;
    QString artist_name_;
    QString genre_;
    //OImage cover_image_;
};

#endif // ALBUM_H
