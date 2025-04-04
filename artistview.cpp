#include "artistview.h"
#include "ui_artistview.h"
#include <QListView>

ArtistView::ArtistView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::ArtistView)
{
    ui_->setupUi(this);
    ui_->listView->setModel(&album_list_model_);

    connect(ui_->listView,
            &QAbstractItemView::doubleClicked,
            this,
            &ArtistView::AlbumDoubleClickedSlot);
}

ArtistView::~ArtistView()
{
    delete ui_;
}

void ArtistView::SetData(QList<Album> data, const QString &artist) {
    album_list_model_.SetAlbums(data);
}

void ArtistView::AlbumDoubleClickedSlot(const QModelIndex &album_index) {
    int row = album_index.row();
    emit AlbumDoubleClickedSignal(album_list_model_.at(row));
}

//TODO: set the proper things
void SetAppearance(QListView *list_view) {
    list_view->setAlternatingRowColors(true);
}

const QString &ArtistView::GetCurrentArtist() {
    return current_artist_;
}
