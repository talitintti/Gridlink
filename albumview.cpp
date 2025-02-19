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
