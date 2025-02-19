#include "artistview.h"
#include "ui_artistview.h"

ArtistView::ArtistView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::ArtistView)
{
    ui_->setupUi(this);
}

ArtistView::~ArtistView()
{
    delete ui_;
}

void ArtistView::SetData(const QList<QString> &data) {
    ui_->listWidget->clear();
    for (QString item : data) {
        ui_->listWidget->addItem(item);
    }
}
