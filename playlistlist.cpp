#include "playlistlist.h"
#include "playlistlistmodel.h"
#include <QMenu>
#include <QVBoxLayout>


PlaylistList::PlaylistList(QWidget *parent)
    : QWidget(parent),
    playlists_(nullptr)
{
    auto layout = new QVBoxLayout(this);
    setLayout(layout);

    model_ = new PlaylistListModel(this);

    listview_ = new QListView(this);
    listview_->setContextMenuPolicy(Qt::CustomContextMenu);
    listview_->setModel(model_);

    layout->addWidget(listview_);

    connect(listview_,
            &QAbstractItemView::clicked,
            this,
            &PlaylistList::SendPlaylistClickedSignal);

    connect(listview_,
            &QAbstractItemView::customContextMenuRequested,
            this,
            &PlaylistList::ShowPlaylistContextMenu);

    this->setMaximumWidth(200);


    layout->setContentsMargins(0, 0, 0, 0);
}

PlaylistList::~PlaylistList() {};

void PlaylistList::SetPlaylists(QList<QSharedPointer<Playlist>> *playlists) {
    model_->SetPlaylists(playlists);
    playlists_ = playlists;
}


void PlaylistList::SendPlaylistClickedSignal(const QModelIndex &index) {
    auto row = index.row();
    emit PlaylistClickedSignal(playlists_->at(row)->GetHash());
}

void PlaylistList::ShowPlaylistContextMenu(const QPoint &pos) {
    QModelIndex index = listview_->indexAt(pos);

    if (!index.isValid()) return;

    int row = index.row();

    QString delete_playlist_text = "Delete playlist";
    QString rename_playlist_text = "Rename playlist";

    QMenu menu;
    QAction *delete_playlist_action = menu.addAction(delete_playlist_text);
    QAction *rename_playlist_action = menu.addAction(rename_playlist_text);

    QAction *selectedAction = menu.exec(listview_->viewport()->mapToGlobal(pos));

    if (selectedAction == delete_playlist_action) {
        emit DeletingPlaylist(row);
    }

    if (selectedAction == rename_playlist_action) {
        //datahandler_.RenamePlaylist(uint row);
    }

}

void PlaylistList::PlaylistUpdate() {
    model_->ResetModel();
}

void PlaylistList::PlaylistAboutToBeRemoved(unsigned row) {
    model_->PlaylistAboutToBeRemoved(row);
}

void PlaylistList::PlaylistRemoved() {
    model_->PlaylistRemoved();
}

void PlaylistList::ClearSelection() {
    listview_->clearSelection();
}
