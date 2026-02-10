#ifndef ALBUM_H
#define ALBUM_H
#include <QList>
#include "songcollection.h"
#include "enums.h"
#include <QDebug>
#include <QImage>

class Album : public SongCollection
{
public:
    using SongCollection::SongCollection;

    QString GetAlbumArtist();
    SONGCOLLECTION_TYPE Identify() const override;

private:
    QString artist_name_;
    QString genre_;
};

#endif // ALBUM_H
