#include "albumview.h"
#include "ui_albumview.h"
#include <QTableView>
#include <QAbstractItemModel>
#include <QRgb>
#include "appearance.h"
#include <QMenu>

AlbumView::AlbumView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::AlbumView)
{
    ui_->setupUi(this);
    song_table_model_ = new SongTableModel(this);
    ui_->tableView->setModel(song_table_model_);
    SetTableAppearance(ui_->tableView);

    connect(ui_->tableView,
            &QTableView::doubleClicked,
            this,
            &AlbumView::SongChosenForPlaySlot);

    ui_->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_->tableView,
            &QTableView::customContextMenuRequested,
            this,
            &AlbumView::OnTableContextMenu);
}

AlbumView::~AlbumView()
{
    delete ui_;
}

void AlbumView::SetAlbum(const Album &album) {
    album_ = album;

    song_table_model_->SetSongs(album_.GetSongs());

    // Let's show the picture
    if (album_.HasCoverData()) {
        auto label = ui_->label_album_pic;
        auto &image = album.GetCover();

        QImage scaled_image = image.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(scaled_image));
    }
}


void AlbumView::SongChosenForPlaySlot(const QModelIndex &q_index) {
    if (!q_index.isValid()) {
        return;
    }
    unsigned row = q_index.row();

    emit SongChosenForPlaySignal(album_.GetSongs(), row);
}

// Takes a song that is playing
void AlbumView::InformSongPlaying(const Song &song) {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetSongAsPlaying(song.GetHash());

}

void AlbumView::InformSongNotPlaying() {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetPlayingAsStopped();
}

const Album &AlbumView::GetCurrentAlbum() {
    return album_;
}

void AlbumView::OnTableContextMenu(const QPoint &pos) {
    QModelIndex index = ui_->tableView->indexAt(pos);

    if (!index.isValid())
        return;

    int row = index.row();

    QString add_to_playlist_text = "Add to playlist";
    QString add_to_queue_text = "Add to queue";

    auto song = album_.GetSongs().at(row);
    QList<Song> list;
    list.append(song);

    QMenu menu;
    QAction *add_to_queue_action = menu.addAction(add_to_queue_text, this, [=]() {
        emit UserAddingSongsToQueue(list);
    });

    QMenu *playlistmenu = new QMenu(add_to_playlist_text, &menu);

    auto playlists = playlist_provider_();
    if (playlist_provider_) {
        for (const auto &playlist : std::as_const(*playlists)) {
            playlistmenu->addAction(playlist->GetName(), this, [=]() {
                emit UserAddingSongsToPlaylist(list, playlist.data());
            });
        }
    }

    menu.addMenu(playlistmenu);

    QAction *selectedAction = menu.exec(ui_->tableView->viewport()->mapToGlobal(pos));
}

