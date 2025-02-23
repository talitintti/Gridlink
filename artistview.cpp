#include "artistview.h"
#include "ui_artistview.h"

ArtistView::ArtistView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::ArtistView)
{
    ui_->setupUi(this);
    ui_->listView->setModel(&album_list_model_);
}

ArtistView::~ArtistView()
{
    delete ui_;
}

void ArtistView::SetData(QList<Album> data) {
    album_list_model_.setAlbums(data);
}
