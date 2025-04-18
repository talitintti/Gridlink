#ifndef SONGCOLLECTIONVIEW_H
#define SONGCOLLECTIONVIEW_H

#include <QWidget>
#include "songtablemodel.h"
#include <QTableView>
#include "playlist.h"

namespace Ui {
class SongCollectionView;
}

class SongCollectionView : public QWidget
{
    Q_OBJECT

public:
    explicit SongCollectionView(QWidget *parent = nullptr);
    ~SongCollectionView();

    void SetSongCollection(const SongCollection *);
    void InformSongPlaying(const Song &);
    void InformSongNotPlaying();
    const SongCollection *GetCurrentSongCollection();

    std::function<QList<QSharedPointer<Playlist>>*()> playlist_provider_;

signals:
    void SongChosenForPlaySignal(const QList<Song> &songs, unsigned index);
    void UserAddingSongsToQueue(const QList<Song> &);
    void UserAddingSongsToPlaylist(const QList<Song> &, const Playlist *);
    void UserDeletingFromPlaylist(const QList<Song> &, const Playlist*);

private slots:
    void SongChosenForPlaySlot(const QModelIndex &);
    void OnTableContextMenu(const QPoint &pos);

private:
    Ui::SongCollectionView *ui_;
    SongTableModel *song_table_model_ = nullptr;
    const SongCollection *songcollection_;

    void ForegroundUpdateForItem(const Song &);
};

#endif // SONGCOLLECTIONVIEW_H
