#include "albumview.h"
#include "ui_albumview.h"
#include <QTableView>
#include <QAbstractItemModel>
#include <QRgb>
#include "appearance.h"

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
        const OImage &cover = album.GetCover();

        QImage image(cover.GetImageData(),
                     cover.GetWidth(),
                     cover.GetHeight(),
                     cover.GetFormat());

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
