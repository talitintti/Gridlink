#include "albumview.h"
#include "ui_albumview.h"

AlbumView::AlbumView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::AlbumView)
{
    ui_->setupUi(this);
}

AlbumView::~AlbumView()
{
    delete ui_;
}

void AlbumView::SetData(const Album &album) {
    if (!song_table_model_) {
        song_table_model_ = new SongTableModel()
    }

}
