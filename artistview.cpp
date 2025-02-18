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

void ArtistView::show_data(const QList<QString> &data) {
    for (QString item : data) {
        ui->listWidget->addItem(item);
    }
}
