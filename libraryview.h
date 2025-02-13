#ifndef LIBRARYVIEW_H
#define LIBRARYVIEW_H

#include <QWidget>

namespace Ui {
class LibraryView;
}

class LibraryView : public QWidget
{
    Q_OBJECT


public:
    explicit LibraryView(QWidget *parent = nullptr);
    void show_data(const QVector<QString> &data);
    ~LibraryView();

private:
    Ui::LibraryView *ui;

};

#endif // LIBRARYVIEW_H
