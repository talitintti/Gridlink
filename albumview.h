#ifndef ALBUMVIEW_H
#define ALBUMVIEW_H

#include <QWidget>

namespace Ui {
class AlbumView;
}

class AlbumView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumView(QWidget *parent = nullptr);
    ~AlbumView();

private:
    Ui::AlbumView *ui;
};

#endif // ALBUMVIEW_H
