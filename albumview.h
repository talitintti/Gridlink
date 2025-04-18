#ifndef ALBUMVIEW_H
#define ALBUMVIEW_H

#include <QWidget>
#include "album.h"
#include "songtablemodel.h"
#include <QTableView>
#include "playlist.h"

namespace Ui {
class AlbumView;
}

class AlbumView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumView(QWidget *parent = nullptr);
    ~AlbumView();

    void SetAlbum(const Album &album);
    void InformSongPlaying(const Song &);
    void InformSongNotPlaying();
    const Album &GetCurrentAlbum();

    std::function<QList<QSharedPointer<Playlist>>*()> playlist_provider_;

signals:
    void SongChosenForPlaySignal(const QList<Song> &songs, unsigned index);
    void UserAddingSongsToQueue(const QList<Song> &);
    void UserAddingSongsToPlaylist(const QList<Song> &, Playlist *);

private slots:
    void SongChosenForPlaySlot(const QModelIndex &);
    void OnTableContextMenu(const QPoint &pos);

private:
    Ui::AlbumView *ui_;
    SongTableModel *song_table_model_ = nullptr;
    Album album_;

    void ForegroundUpdateForItem(const Song &);
};

#endif // ALBUMVIEW_H
