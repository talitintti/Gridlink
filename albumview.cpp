#include "albumview.h"
#include "ui_albumview.h"

AlbumView::AlbumView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlbumView)
{
    ui->setupUi(this);
}

AlbumView::~AlbumView()
{
    delete ui;
}
