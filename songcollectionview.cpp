#include "songcollectionview.h"
#include "ui_songcollectionview.h"

#include <QTableView>
#include <QAbstractItemModel>
#include <QRgb>
#include "appearance.h"
#include <QMenu>

SongCollectionView::SongCollectionView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::SongCollectionView)
{
    ui_->setupUi(this);
    song_table_model_ = new SongTableModel(this);
    ui_->tableView->setModel(song_table_model_);
    SetTableAppearance(ui_->tableView);

    connect(ui_->tableView,
            &QTableView::doubleClicked,
            this,
            &SongCollectionView::SongChosenForPlaySlot);

    ui_->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui_->tableView,
            &QTableView::customContextMenuRequested,
            this,
            &SongCollectionView::OnTableContextMenu);
}

SongCollectionView::~SongCollectionView()
{
    delete ui_;
}

void SongCollectionView::SetSongCollection(const SongCollection *songcollection) {
    songcollection_ = songcollection;

    song_table_model_->SetSongs(songcollection_->GetSongs());

    // Let's show the picture
    if (songcollection_->HasCoverData()) {
        auto label = ui_->label_album_pic;
        const OImage &cover = songcollection->GetCover();

        QImage image(cover.GetImageData(),
                     cover.GetWidth(),
                     cover.GetHeight(),
                     cover.GetFormat());

        QImage scaled_image = image.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(scaled_image));
    }
}


void SongCollectionView::SongChosenForPlaySlot(const QModelIndex &q_index) {
    if (!q_index.isValid()) {
        return;
    }
    unsigned row = q_index.row();

    emit SongChosenForPlaySignal(songcollection_->GetSongs(), row);
}

// Takes a song that is playing
void SongCollectionView::InformSongPlaying(const Song &song) {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetSongAsPlaying(song.GetHash());

}

void SongCollectionView::InformSongNotPlaying() {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetPlayingAsStopped();
}

const SongCollection *SongCollectionView::GetCurrentSongCollection() {
    return songcollection_;
}

void SongCollectionView::OnTableContextMenu(const QPoint &pos) {
    QModelIndex index = ui_->tableView->indexAt(pos);

    if (!index.isValid())
        return;

    int row = index.row();

    QString add_to_playlist_text = "Add to playlist";
    QString add_to_queue_text = "Add to queue";
    QString remove_from_playlist_text = "Remove from playlist";

    auto song = songcollection_->GetSongs().at(row);
    QList<Song> list = {song};

    // Main menu
    QMenu menu;
    QAction *add_to_queue_action = menu.addAction(add_to_queue_text, this, [=]() {
        emit UserAddingSongsToQueue(list);
    });

    if (songcollection_->Identify() == SONGCOLLECTION_TYPE::PLAYLIST) {
        QAction *remove_from_playlist = menu.addAction(remove_from_playlist_text, this, [=] {
            emit UserDeletingFromPlaylist(list, dynamic_cast<const Playlist *>(songcollection_));
        });
    }

    // Sub menu for adding to playlists
    QMenu *playlistmenu = new QMenu(add_to_playlist_text, &menu);

    // adding an item for adding new playlist
    playlistmenu->addAction("+[Add new playlist]", this, [=]() {
        emit ShowPlaylistAddDialog();
    });

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

