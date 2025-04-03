#ifndef MIMEDATA_H
#define MIMEDATA_H

#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QString>
#include <QIODevice>
#include "album.h"


class AlbumMimeData : public QMimeData {
    Q_OBJECT
public:
    explicit AlbumMimeData(Album* album) {
        // Serialize the Album pointer's address into QByteArray
        QByteArray serialized ;
        QDataStream stream(&serialized, QIODevice::WriteOnly);
        stream << reinterpret_cast<quintptr>(album);  // Convert pointer to integer
        album_data_ = serialized;
    }

    Album *albumPointer() const {
        QByteArray serialized = album_data_;
        QDataStream stream(&serialized, QIODevice::ReadOnly);
        quintptr address;
        stream >> address;
        return reinterpret_cast<Album*>(address);  // Convert integer back to pointer
    }

    // Override formats method
    QStringList formats() const override {
        return { "application/x-album-pointer" };
    }

    //// Override data method to return the serialized pointer
    //QByteArray data(const QString &format) const override {
    //    if (format == "application/x-album-pointer") {
    //        return album_data_;  // Return the serialized album data as a QByteArray
    //    }
    //    return QByteArray();  // Return empty QByteArray if the format doesn't match
    //}

private:
    QByteArray album_data_;  // Store the serialized album pointer
};
#endif // MIMEDATA_H
