#include "searchview.h"
#include "ui_searchview.h"

SearchView::SearchView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchView)
{
    ui->setupUi(this);

}

void SearchView::populateTable(QTableWidget *table) {
}

SearchView::~SearchView()
{
    delete ui;
}
