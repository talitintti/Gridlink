#include "searchview.h"
#include "ui_searchview.h"

SearchView::SearchView(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::SearchView)
{
    ui_->setupUi(this);

}


SearchView::~SearchView()
{
    delete ui_;
}
