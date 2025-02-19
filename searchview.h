#ifndef SEARCHVIEW_H
#define SEARCHVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>


namespace Ui {
class SearchView;
}

class SearchView : public QWidget
{
    Q_OBJECT

public:
    explicit SearchView(QWidget *parent = nullptr);
    ~SearchView();

private:
    Ui::SearchView *ui_;
};

#endif // SEARCHVIEW_H
