#include "libraryview.h"
#include "ui_libraryview.h"
#include <QDebug>


LibraryView::LibraryView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::LibraryView)
{
    ui_->setupUi(this);
    ui_->listWidget->setDragEnabled(true);

    connect(ui_->listWidget,
            &QListWidget::itemDoubleClicked,
            this,
            &LibraryView::ArtistDoubleClickedSlot);
}

LibraryView::~LibraryView()
{
    delete ui_;
}

void LibraryView::SetData(const QList<QString> &data) {
    ui_->listWidget->clear();
    for (const QString &item : data) {
        ui_->listWidget->addItem(item);
    }
}

void LibraryView::ArtistDoubleClickedSlot(QListWidgetItem *item) {
    const QString artist_name = item->text();
    emit ArtistDoubleClickedSignal(artist_name);
}
