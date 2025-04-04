#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QWidget>
#include "playlist.h"
#include "songtablemodel.h"
#include <QTableView>

namespace Ui {
class PlaylistView;
}

class PlaylistView : public QWidget
{
    Q_OBJECT

public:
    explicit PlaylistView(QWidget *parent = nullptr);
    ~PlaylistView();

    void SetPlaylist(const Playlist &playlist);
    void InformSongPlaying(const Song &);
    void InformSongNotPlaying();
    const Playlist &GetCurrentPlaylist();

signals:
    void SongChosenForPlaySignal(const QList<Song> &songs, unsigned index);

private slots:
    void SongChosenForPlaySlot(const QModelIndex &);

private:
    Ui::PlaylistView *ui_;
    SongTableModel *song_table_model_ = nullptr;
    Playlist playlist_;

    void SetTableAppearance(QTableView *table);
    void ForegroundUpdateForItem(const Song &);
};

#endif // PLAYLISTVIEW_H
