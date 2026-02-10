#ifndef PLAYLISTLIST_H
#define PLAYLISTLIST_H

#include <QObject>
#include <QWidget>
#include <QListView>
#include "playlist.h"
#include "playlistlistmodel.h"

namespace Ui {
class PlaylistList;
}

class PlaylistList : public QWidget
{
    Q_OBJECT


public:
    explicit PlaylistList(QWidget *parent = nullptr);
    ~PlaylistList();

    void SetPlaylists(QList<QSharedPointer<Playlist>>*);
    void SelectPlaylist(Playlist &playlist);
signals:
    void DeletingPlaylist(unsigned row);
    void PlaylistClickedSignal(size_t hash);

private slots:
    void ShowPlaylistContextMenu(const QPoint &pos);
    void SendPlaylistClickedSignal(const QModelIndex &index);

public slots:
    void PlaylistUpdate();
    void ClearSelection();
    void PlaylistAboutToBeRemoved(unsigned row);
    void PlaylistRemoved();

private:
    PlaylistListModel *model_;
    QListView *listview_;
    QList<QSharedPointer<Playlist>> *playlists_;
};

#endif // PLAYLISTLIST_H
