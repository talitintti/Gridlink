#include "playlistview.h"
#include "ui_playlistview.h"
#include <QTableView>
#include <QAbstractItemModel>
#include <QRgb>
#include "appearance.h"

PlaylistView::PlaylistView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::PlaylistView)
{
    ui_->setupUi(this);
    song_table_model_ = new SongTableModel(this);
    ui_->tableView->setModel(song_table_model_);
    SetTableAppearance(ui_->tableView);

    connect(ui_->tableView,
            &QTableView::doubleClicked,
            this,
            &PlaylistView::SongChosenForPlaySlot);
}

PlaylistView::~PlaylistView()
{
    delete ui_;
}


void PlaylistView::SetPlaylist(const Playlist &playlist) {
    playlist_ = playlist; // we do actually want to copy here because song order might be shuffled

    song_table_model_->SetSongs(playlist_.GetSongs());

    // Let's set the picture
    if (playlist_.HasCoverData()) {
        auto label = ui_->label_playlist_pic;
        const OImage &cover = playlist.GetCover();

        QImage image(cover.GetImageData(),
                     cover.GetWidth(),
                     cover.GetHeight(),
                     cover.GetFormat());

        QImage scaled_image = image.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(scaled_image));
    }
}


void PlaylistView::SongChosenForPlaySlot(const QModelIndex &q_index) {
    if (!q_index.isValid()) {
        return;
    }
    unsigned row = q_index.row();

    emit SongChosenForPlaySignal(playlist_.GetSongs(), row);
}

// Takes a song that is playing
void PlaylistView::InformSongPlaying(const Song &song) {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetSongAsPlaying(song.GetHash());

}

void PlaylistView::InformSongNotPlaying() {
    // Tell the songtablemodel that a song is playing
    song_table_model_->SetPlayingAsStopped();
}

const Playlist &PlaylistView::GetCurrentPlaylist() {
    return playlist_;
}
