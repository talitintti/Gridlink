#include "libraryview.h"
#include "ui_libraryview.h"
#include <QDebug>

LibraryView::LibraryView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LibraryView)
{
    ui->setupUi(this);
}

LibraryView::~LibraryView()
{
    delete ui;
}

void LibraryView::show_data(const QVector<QString> &data) {
    for (QString item : data) {
        ui->listWidget->addItem(item);
    }
}
