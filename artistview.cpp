#include "artistview.h"
#include "ui_artistview.h"

ArtistView::ArtistView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ArtistView)
{
    ui->setupUi(this);
}

ArtistView::~ArtistView()
{
    delete ui;
}
