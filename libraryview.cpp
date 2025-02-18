#include "libraryview.h"
#include "ui_libraryview.h"
#include "mainwindow.h"
#include <QDebug>

LibraryView::LibraryView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LibraryView)
{
    ui->setupUi(this);
    ui->listWidget->setDragEnabled(true);

    connect(ui->listWidget,
            &QListWidget::itemDoubleClicked,
            this,
            &LibraryView::ArtistDoubleClickedSlot);
}

LibraryView::~LibraryView()
{
    delete ui;
}

void LibraryView::show_data(const QList<QString> &data) {
    for (QString item : data) {
        ui->listWidget->addItem(item);
    }
}

void LibraryView::ArtistDoubleClickedSlot(QListWidgetItem *item) {
    const QString artist_name = item->text();
    emit ArtistDoubleClickedSignal(artist_name);
}
