#ifndef ARTISTVIEW_H
#define ARTISTVIEW_H

#include <QWidget>

namespace Ui {
class ArtistView;
}

class ArtistView : public QWidget
{
    Q_OBJECT

public:
    explicit ArtistView(QWidget *parent = nullptr);
    ~ArtistView();

private:
    Ui::ArtistView *ui;
};

#endif // ARTISTVIEW_H
